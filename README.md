# OTP-Gated Server-Side Decryption Service

A full-stack application featuring client-side Web Crypto encryption, a high-performance C/Mongoose backend, and an OTP-gated server-side decryption workflow.

---

## Architecture Overview

This project provides a secure client-to-server decryption workflow:
1. Local Encryption (Frontend): Sensitive payloads are encrypted locally in the browser using AES-256-GCM before transmission.
2. Binary Transit: Outputs (Salt, Nonce, Auth Tag, Ciphertext) are packed into a unified binary format and Base64-encoded to ensure clean payload transport.
3. OTP Authentication & Decryption (Backend): The C backend receives the payload, initiates an OTP session, and performs OpenSSL decryption on the server only after two-factor validation succeeds.

---

## Project Structure
```
.
├── backend/               # C17 Mongoose / OpenSSL server
│   ├── build/             # Output binaries and CMake build files
│   ├── database/          # Local persistent storage
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
   cd backend
   cmake -B build -G Ninja
   cmake --build build
   cp .env build/.env
   cd build
   ./EncryptionAPI.exe

The backend service runs on http://localhost:3000.

---

### 3. Frontend Setup & Run

Open a standard Terminal / PowerShell window:

1. Install npm packages:
   cd frontend
   npm install

2. Configure environment:
   Create a .env in frontend/:
   VITE_API_BASE_URL=http://localhost:3000

3. Start development server:
   npm run dev

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