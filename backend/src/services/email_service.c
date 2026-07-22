#include "services/email_service.h"
#include <curl/curl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct upload_status {
    size_t bytes_read;
    const char *payload;
};

static size_t payload_source(char *ptr, size_t size, size_t nmemb, void *userp) {
    struct upload_status *upload_ctx = (struct upload_status *)userp;
    size_t len = strlen(upload_ctx->payload);

    if (upload_ctx->bytes_read >= len) {
        return 0;
    }

    size_t to_copy = len - upload_ctx->bytes_read;
    if (to_copy > size * nmemb) {
        to_copy = size * nmemb;
    }

    memcpy(ptr, upload_ctx->payload + upload_ctx->bytes_read, to_copy);
    upload_ctx->bytes_read += to_copy;

    return to_copy;
}

int send_otp_email(const char *to_email, const char *otp_code) {
    const char *smtp_url = getenv("SMTP_URL");
    const char *smtp_user = getenv("SMTP_USER");
    const char *smtp_pass = getenv("SMTP_PASSWORD");

    if (!smtp_url) smtp_url = "smtp://smtp.gmail.com:587";
    if (!smtp_user || !smtp_pass) {
        printf("[SMTP ERROR] Missing SMTP_USER or SMTP_PASSWORD environment variables\n");
        return -1;
    }

    CURL *curl = curl_easy_init();
    if (!curl) {
        printf("[SMTP ERROR] Failed to initialize curl easy handle\n");
        return -1;
    }

    char payload_text[512];
    snprintf(payload_text, sizeof(payload_text),
             "To: %s\r\n"
             "From: %s\r\n"
             "Subject: Your Verification OTP Code\r\n"
             "Content-Type: text/plain; charset=utf-8\r\n"
             "\r\n"
             "Your OTP verification code is: %s\r\n"
             "This code will expire shortly.\r\n",
             to_email, smtp_user, otp_code);

    struct upload_status upload_ctx = { 0, payload_text };
    struct curl_slist *recipients = NULL;

    curl_easy_setopt(curl, CURLOPT_URL, smtp_url);
    curl_easy_setopt(curl, CURLOPT_USERNAME, smtp_user);
    curl_easy_setopt(curl, CURLOPT_PASSWORD, smtp_pass);

    curl_easy_setopt(curl, CURLOPT_USE_SSL, (long)CURLUSESSL_ALL);
    
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

    curl_easy_setopt(curl, CURLOPT_MAIL_FROM, smtp_user);
    recipients = curl_slist_append(recipients, to_email);
    curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);

    curl_easy_setopt(curl, CURLOPT_READFUNCTION, payload_source);
    curl_easy_setopt(curl, CURLOPT_READDATA, &upload_ctx);
    curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);

    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

    CURLcode res = curl_easy_perform(curl);

    if (res != CURLE_OK) {
        printf("[SMTP ERROR] curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
    } else {
        printf("[SMTP SUCCESS] OTP email sent successfully to %s\n", to_email);
    }

    curl_slist_free_all(recipients);
    curl_easy_cleanup(curl);

    return (res == CURLE_OK) ? 0 : -1;
}