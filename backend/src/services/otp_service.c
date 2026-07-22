#include "services/otp_service.h"

#include <openssl/rand.h>
#include <openssl/crypto.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <pthread.h>

typedef struct {
    char *session_id;
    char *otp;
    char *plaintext;
    char *recipient_type;
    char *recipient;
    time_t expires_at;
    int active;
} OtpSession;

static OtpSession sessions[32];
static pthread_mutex_t session_mutex = PTHREAD_MUTEX_INITIALIZER;

static void otp_session_clear(OtpSession *session)
{
    if (!session) return;
    free(session->session_id);
    free(session->otp);
    free(session->plaintext);
    free(session->recipient_type);
    free(session->recipient);
    memset(session, 0, sizeof(*session));
}

static int safe_str_eq(const char *a, const char *b) 
{
    if (!a || !b) return 0;
    size_t len_a = strlen(a);
    size_t len_b = strlen(b);
    if (len_a != len_b) return 0;
    return CRYPTO_memcmp(a, b, len_a) == 0;
}

static char *generate_numeric_otp(void)
{
    unsigned int value = 0;
    do {
        if (RAND_bytes((unsigned char *)&value, sizeof(value)) != 1) {
            return NULL;
        }
    } while (value >= (0xFFFFFFFF - (0xFFFFFFFF % 1000000)));

    unsigned int otp_value = value % 1000000;

    char *otp = malloc(7);
    if (!otp) return NULL;

    snprintf(otp, 7, "%06u", otp_value);
    return otp;
}

static char *generate_session_id(void)
{
    unsigned char random_bytes[16];
    if (RAND_bytes(random_bytes, sizeof(random_bytes)) != 1) {
        return NULL;
    }

    char *hex_id = malloc(33);
    if (!hex_id) return NULL;

    for (int i = 0; i < 16; i++) {
        sprintf(hex_id + (i * 2), "%02x", random_bytes[i]);
    }
    hex_id[32] = '\0';

    return hex_id;
}

char *otp_create_session(
    const char *recipient_type,
    const char *recipient,
    const char *plaintext)
{
    if (!recipient_type || !recipient || !plaintext) {
        return NULL;
    }

    char *otp = generate_numeric_otp();
    char *session_id = generate_session_id();
    if (!otp || !session_id) {
        free(otp);
        free(session_id);
        return NULL;
    }

    pthread_mutex_lock(&session_mutex);

    OtpSession *slot = NULL;
    time_t now = time(NULL);

    for (size_t i = 0; i < sizeof(sessions) / sizeof(sessions[0]); ++i) {
        if (sessions[i].active && sessions[i].expires_at < now) {
            otp_session_clear(&sessions[i]);
        }

        if (!sessions[i].active && !slot) {
            slot = &sessions[i];
        }
    }

    if (!slot) {
        pthread_mutex_unlock(&session_mutex);
        free(otp);
        free(session_id);
        return NULL;
    }

    slot->session_id = session_id;
    slot->otp = otp;
    slot->plaintext = strdup(plaintext);
    slot->recipient_type = strdup(recipient_type);
    slot->recipient = strdup(recipient);
    slot->expires_at = now + 300;
    slot->active = 1;

    if (!slot->plaintext || !slot->recipient_type || !slot->recipient) {
        otp_session_clear(slot);
        pthread_mutex_unlock(&session_mutex);
        return NULL;
    }

    char *returned_id = strdup(slot->session_id);

    pthread_mutex_unlock(&session_mutex);

    return returned_id;
}

char *otp_verify_session(
    const char *session_id,
    const char *otp)
{
    if (!session_id || !otp) {
        return NULL;
    }

    pthread_mutex_lock(&session_mutex);

    time_t now = time(NULL);
    for (size_t i = 0; i < sizeof(sessions) / sizeof(sessions[0]); ++i) {
        OtpSession *slot = &sessions[i];
        if (!slot->active) {
            continue;
        }

        if (slot->expires_at < now) {
            otp_session_clear(slot);
            continue;
        }

        if (safe_str_eq(slot->session_id, session_id) && safe_str_eq(slot->otp, otp)) {
            char *plaintext_copy = strdup(slot->plaintext ? slot->plaintext : "");
            otp_session_clear(slot);
            pthread_mutex_unlock(&session_mutex);
            return plaintext_copy;
        }
    }

    pthread_mutex_unlock(&session_mutex);
    return NULL;
}

char *otp_get_code_by_session_id(const char *session_id)
{
    if (!session_id) return NULL;

    pthread_mutex_lock(&session_mutex);

    time_t now = time(NULL);
    for (size_t i = 0; i < sizeof(sessions) / sizeof(sessions[0]); ++i) {
        OtpSession *slot = &sessions[i];
        if (slot->active) {
            if (slot->expires_at < now) {
                otp_session_clear(slot);
                continue;
            }

            if (safe_str_eq(slot->session_id, session_id)) {
                char *otp_copy = strdup(slot->otp);
                pthread_mutex_unlock(&session_mutex);
                return otp_copy;
            }
        }
    }

    pthread_mutex_unlock(&session_mutex);
    return NULL;
}