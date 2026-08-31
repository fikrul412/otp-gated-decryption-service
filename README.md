# OTP-Gated Server-Side Decryption Service

A full-stack application featuring client-side Web Crypto encryption, a high-performance C/Mongoose backend, and an OTP-gated server-side decryption workflow.

---

## Architecture Overview

This project implements a secure client-to-server decryption workflow designed to protect sensitive data throughout transmission:

1. Client-Side Encryption (Frontend): Sensitive data is encrypted directly in the user's browser using AES-256-GCM before being sent to the server.
2. Binary Payload Transmission: The encryption components—including the Salt, Nonce, Authentication Tag, and Ciphertext—are combined into a single binary structure and Base64-encoded for reliable and consistent transmission.
3. OTP-Verified Server-Side Decryption (Backend): The C-based backend receives the encrypted payload and initiates OTP verification. OpenSSL decryption is performed on the server only after the two-factor authentication process has been successfully completed.

---
## System Workflow Diagram

![alt text](SystemWorkflowDiagram.png)
1. **Encryption Phase (Client-Side)**

   - The user inputs the plaintext `text`, destination `recipient_type`, `recipient` email/address, and a secret `password`.
   - The password is derived into an `aes_key` using **PBKDF2**.
   - The payload is encrypted locally using **AES-256-GCM** to produce an `Encrypted Package` (containing salt, nonce, tag, and ciphertext).

2. **Decryption Phase (Client to Server)**
   - The client submits the `password` and `encrypted_pkg` to the C server.
   - The backend runs **PBKDF2** using the submitted password and salt from the package to derive the `aes_key`.
   - The backend decrypts the package using **AES-256-GCM** to retrieve the metadata (`text`, `recipient_type`, `recipient`).
   - An **OTP Generation** event triggers: a time-sensitive OTP code is generated, stored in temporary session state, and dispatched to the `recipient` via SMTP over SMTPS.

3. **Verification Phase (OTP Validation)**
   - The user enters the `OTP Numbers` received in their email into the frontend interface.
   - The client posts the OTP to the server for **OTP Verification**.
   - Upon successful verification, the backend returns the decrypted `text` (plaintext) back to the client.
## Project Structure
```
.
├── backend/               # C17 Mongoose / OpenSSL server
│   ├── build/             # Output binaries and CMake build files
│   ├── external/          # Embedded dependencies (cJSON, Mongoose)
│   ├── include/           # Header files
│   └── src/               # Core server logic and OpenSSL routines
│
└── frontend/              # SvelteKit + Vite web client
    ├── src/               # UI components, pages, and Web Crypto services
    │   ├── lib/           # Shared components and API handlers
    │   └── routes/        # App routing pages (/decrypt)
    └── static/            # Static assets
```

## Technical Stack

### Backend
- Language: C (C17)
- Web Server: Mongoose HTTP Server
- Cryptography: OpenSSL (libcrypto, libssl)
- HTTP Utilities: libcurl
- JSON Parser: cJSON
- Build System: CMake (Ninja / MinGW)

### Frontend
- Framework: Svelte 5 / SvelteKit
- Build Engine: Vite
- Cryptography: Web Crypto API (AES-GCM, PBKDF2)
- Runtime: Node.js (v18+)

---

## Quick Start & Setup

### 1. Prerequisites
- MSYS2 (with UCRT64 environment) for Windows C compilation.
- Node.js (v18 or higher) and npm.

---

### 2. Backend Setup & Run

Open the MSYS2 UCRT64 terminal:

1. Install dependencies:
```
pacman -S --needed \
  mingw-w64-ucrt-x86_64-toolchain \
  mingw-w64-ucrt-x86_64-cmake \
  mingw-w64-ucrt-x86_64-ninja \
  mingw-w64-ucrt-x86_64-openssl \
  mingw-w64-ucrt-x86_64-curl
```
2. Configure environment:
   Create a .env file in backend/ (refer to backend/README.md for details).

3. Build and execute:
```
   cd backend
   cmake -B build -G Ninja
   cmake --build build
   cp .env build/.env
   cd build
   ./EncryptionAPI.exe
```
The backend service runs on http://localhost:3000.

---

### 3. Frontend Setup & Run

Open a standard Terminal / PowerShell window:

1. Install npm packages:
```
   cd frontend
   npm install
```
2. Configure environment:
   Create a .env in frontend/:
```
   PUBLIC_API_BASE_URL=http://localhost:3000
```
3. Start development server:
```
   npm run dev
```
The web app will be live at http://localhost:5173.

---

## API & Communication Flow

| Endpoint | Method | Purpose |
| :--- | :--- | :--- |
| /health | GET | Server availability check |
| /decrypt | POST | Initiates decryption flow & triggers OTP generation |
| /otp/verify | POST | Validates OTP input and returns decrypted plaintext |

---

## Documentation

For component-specific configurations, detailed dependencies, and internal architecture notes, refer to individual component guides:
- Backend Documentation: backend/README.md
- Frontend Documentation: frontend/README.md