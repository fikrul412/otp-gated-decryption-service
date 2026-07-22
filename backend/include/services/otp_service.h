#ifndef OTP_SERVICE_H
#define OTP_SERVICE_H

/**
 * Creates a temporary OTP session for a decrypted payload.
 * Returns a newly allocated session ID string on success, or NULL on failure.
 * The caller must free the returned session ID.
 */
char *otp_create_session(
    const char *recipient_type,
    const char *recipient,
    const char *plaintext
);

/**
 * Verifies an OTP code for an existing session.
 * Returns a newly allocated plaintext string on success, or NULL on failure.
 * The caller must free the returned plaintext.
 */
char *otp_verify_session(
    const char *session_id,
    const char *otp
);

/**
 * Fetches the active OTP code associated with a session ID.
 * Returns a newly allocated OTP string on success, or NULL on failure/expiry.
 * The caller must free the returned string.
 */
char *otp_get_code_by_session_id(
    const char *session_id
);

#endif // OTP_SERVICE_H