# OTP-Protected Text Encryption Designs

Lightweight C backend + Svelte frontend demo that performs AES-256-CBC encryption of small JSON payloads and exposes simple HTTP endpoints to encrypt/decrypt text protected by an OTP flow (OTP verification currently stubbed).

This repository is a work-in-progress prototype demonstrating an architecture for OTP-protected message encryption where the encrypted blob contains a stringified JSON payload (message + recipient metadata). The frontend is a small Svelte app used to call the C backend.

**Stack**
- **Backend:** C (C17), CMake, OpenSSL (libcrypto), cJSON, Mongoose (embedded HTTP), optional SQLite for DB code paths.
- **Frontend:** Svelte (Vite), Node.js, npm

Quick start
- Build & run backend (from `backend/`):

```powershell
cmake -S . -B build -G "MinGW Makefiles"
cmake --build build
# Run the server (adjust path/name if different)
.\build\EncryptionAPI.exe
```

- Start frontend (from `frontend/`):

```bash
npm install
npm run dev
```

API Summary (current)
- POST `/encrypt` — Request JSON:
  - `method` (string), `message` (string), `recipient_type` (string), `recipient` (string)
  - Response JSON: `{ "encrypted_data": string, "generated_key": string }` (both base64)
- POST `/decrypt` — Request JSON:
  - `method` (string), `encrypted_data` (string, base64), `otp` (string), `encryption_key` (string, base64 key+iv)
  - Response JSON: `{ "message": string }` — server logs recipient metadata to its console

Notes
- OTP verification is scaffolded but currently behaves as a stub in `otp_service` — it will need production implementation.
- On Windows you may need to point CMake to an OpenSSL development installation (headers + libs). See `backend/README.md` for details.

If you want me to add example curl/Postman requests or expand OTP handling and tests, tell me which to prioritize.
# OTP-Protected-Text-Encryption-Designs
A collection of reference implementations demonstrating different approaches to building an OTP-protected encryption service. The repository explores multiple key management strategies on C.

```
backend/
│
├── include/
│   ├── controllers/
│   │   ├── decrypt_controller.h
│   │   ├── encrypt_controller.h
│   │   ├── health_controller.h
│   │   └── otp_controller.h
│   │
│   ├── crypto/
│   │   ├── aes.h
│   │   ├── random.h
│   │   └── base64.h
│   │
│   ├── models/
│   │   ├── encrypt_request.h
│   │   ├── encrypt_response.h
│   │   ├── decrypt_request.h
│   │   ├── decrypt_response.h
│   │   ├── otp_request.h
│   │   └── otp_response.h
│   │
│   ├── services/
│   │   ├── encrypt_service.h
│   │   ├── decrypt_service.h
│   │   ├── otp_service.h
│   │   ├── database_service.h
│   │   └── email_service.h
│   │
│   ├── utils/
│   │   └── json_utils.h
│   │
│   ├── router.h
│   └── server.h
│
├── src/
│   ├── controllers/
│   │
│   ├── crypto/
│   │   ├── aes.c
│   │   ├── random.c
│   │   └── base64.c
│   │
│   ├── services/
│   │   ├── encrypt_service.c
│   │   ├── decrypt_service.c
│   │   ├── otp_service.c
│   │   ├── database_service.c
│   │   └── email_service.c
│   │
│   ├── utils/
│   ├── main.c
│   ├── router.c
│   └── server.c
│
├── external/
│   ├── mongoose/
│   ├── cJSON/
│   └── openssl/
│
└── CMakeLists.txt
```
