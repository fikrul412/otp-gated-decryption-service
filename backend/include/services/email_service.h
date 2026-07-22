#ifndef EMAIL_SERVICE_H
#define EMAIL_SERVICE_H

// Sends an OTP code to the target email address using SMTP settings from environment variables.
// Returns 0 on success, -1 on failure.
int send_otp_email(const char *to_email, const char *otp_code);

#endif // EMAIL_SERVICE_H