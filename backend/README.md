# Backend — Encryption API (C)

Brief
- Minimal C backend that exposes HTTP endpoints to encrypt and decrypt small JSON payloads. Uses AES-256-CBC (OpenSSL) and cJSON for payload handling. Mongoose provides the embedded HTTP server.

Tech stack & libs
- C (C17) with CMake
- OpenSSL (libcrypto) — AES, RAND, BIO
- cJSON — JSON parsing/serialization
- Mongoose — small HTTP server

Prerequisites (development)
- A C toolchain (MinGW-w64 on Windows, or gcc/clang on *nix)
- CMake 3.15+
- OpenSSL development libraries (headers + lib). On Windows you may use MSYS2 (`C:\msys64\ucrt64`) or another OpenSSL install and point CMake to the root.

Build
1. Open a terminal in `backend/`.
2. Configure & build:

```powershell
cmake -S . -B build -G "MinGW Makefiles"
cmake --build build
```

If CMake cannot find OpenSSL on Windows, pass variables to `cmake`:

```powershell
cmake -S . -B build -G "MinGW Makefiles" -DOPENSSL_ROOT_DIR="C:/msys64/ucrt64" -DOPENSSL_INCLUDE_DIR="C:/msys64/ucrt64/include" -DOPENSSL_LIBRARIES="C:/msys64/ucrt64/lib"
cmake --build build
```

Run
- Run the produced executable (e.g. `EncryptionAPI.exe` in `build/`). The server listens on port `3000` by default (see server startup logs or `main.c`).

Endpoints (current)
- POST `/encrypt`
  - Request JSON: `{"method":"...","message":"...","recipient_type":"Email|Phone","recipient":"..."}`
  - Response JSON: `{"encrypted_data": "<base64>", "generated_key": "<base64>"}`
  - Notes: `generated_key` contains base64(key || iv) used for decryption; `encrypted_data` contains base64(ciphertext of a stringified JSON payload that includes message and recipient metadata).

- POST `/decrypt`
  - Request JSON: `{"method":"...","encrypted_data":"<base64>","otp":"...","encryption_key":"<base64>"}`
  - Response JSON: `{"message":"..."}`
  - Notes: The server prints `recipient_type` and `recipient` to its console for logging but does not return them in the JSON response.

Other routes
- POST `/otp` — scaffolding present for OTP requests and responses. OTP verification is currently a stub and should be implemented for production usage.

Files of interest
- `src/services/crypto_service.c` — encryption/decryption implementation
- `src/crypto/aes.c` — OpenSSL EVP wrapper
- `src/utlis/json_utils.c` — request parsing / response serialization
- `src/controllers/*` — route handlers

Testing
- Use Postman or curl to exercise `/encrypt` then `/decrypt` using the returned `generated_key`.

Security notes
- This is a prototype. Do not use this code as-is in production: missing OTP hardening, lack of authentication, no rate-limiting, and limited input sanitization.
