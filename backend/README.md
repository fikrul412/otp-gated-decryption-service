# Backend — OTP-Gated Decryption API

This backend handles server-side decryption of encrypted packages, OTP verification, and API requests for the application. Client-side encryption is performed locally before submitting the package and password to this service.

## Tech Stack
- Language: C (C17 standard)
- Build System: CMake (3.20+)
- Compiler: GCC / MinGW-w64
- Cryptography: OpenSSL (libcrypto / libssl)
- HTTP Client: libcurl
- JSON Parsing: cJSON
- Web Server: Mongoose HTTP Server

---

## Directory Structure
```
backend/
├── build/                 # Compiled binaries and CMake build files
├── database/              # Persistence / storage files
├── external/              # Third-party dependencies
│   ├── cJSON/             # cJSON library files
│   └── mongoose/          # Mongoose web server files
├── include/               # Header files
│   ├── controllers/
│   ├── crypto/
│   └── services/
└── src/                   # Source code
    ├── controllers/
    ├── crypto/
    └── services/

```

## Environment Setup (MSYS2 / Windows)

To build this project on Windows, use MSYS2 with the UCRT64 environment.

### 1. Install MSYS2
Download and install MSYS2 from https://www.msys2.org/.

### 2. Update Package Database
Open the MSYS2 UCRT64 terminal and update the package database:
```
  pacman -Syu
```
### 3. Install Required Dependencies
Run the following command to install the C compiler toolchain, CMake, Ninja, OpenSSL, and libcurl:
```
  pacman -S --needed \
    mingw-w64-ucrt-x86_64-toolchain \
    mingw-w64-ucrt-x86_64-cmake \
    mingw-w64-ucrt-x86_64-ninja \
    mingw-w64-ucrt-x86_64-openssl \
    mingw-w64-ucrt-x86_64-curl
```
Note: When prompted during the toolchain installation, press Enter to select all default components.

## Environment Variables (.env Setup)

The application uses environment variables for server options, database connection, and third-party API configurations.

### 1. Create the .env File
In the root directory of the backend folder, create a file named `.env` based on the template below.

### 2. Required Variables Configuration
```
SMTP_URL="smtp://smtp.gmail.com:587"
SMTP_USER="you_email@gmail.com"
SMTP_PASSWORD="yourpassword"
```
> **Note for Gmail Users:** If you are using Google (Gmail) SMTPS, generate an App Password via your [Google Account Security settings](https://myaccount.google.com/apppasswords) and use `smtp.gmail.com` with port `465`.
## Build Instructions

### Using MSYS2 UCRT64 Terminal (Recommended)

1. Navigate to the backend directory:
```
     cd /c/path/to/your/project/backend
```
2. Configure the build using Ninja:
```
     cmake -B build -G Ninja
```
3. Build the project:
```
     cmake --build build
```
### Using PowerShell / Command Prompt (MinGW Makefiles)
```
  cd backend
  cmake -S . -B build -G "MinGW Makefiles"
  cmake --build build
```

## Moving the .env File After Build

When the build is finished, the executable (`EncryptionAPI.exe`) is created inside the `build/` directory.

Because C applications read `.env` relative to their current working execution directory, you must do ONE of the following steps:

### Option A (Recommended for Development):
Copy or move your `.env` file into the `build/` folder where the executable lives:

  Using MSYS2 terminal:
  ```
  cp .env build/.env
  ```
  Using PowerShell:
  ```
  Copy-Item .env build\.env
  ```
### Option B (Running from Root):
If you launch the executable from the root `backend/` directory while referencing the build path, the app will read the `.env` from the root folder:
```
  ./build/EncryptionAPI.exe
```
---

## Running the Server

Start the compiled executable:

### MSYS2 UCRT64:
```
  cd build
  ./EncryptionAPI.exe
```
### PowerShell:
```
  cd build
  .\EncryptionAPI.exe
```
By default, the HTTP server listens on port 3000 (or the custom PORT).


## API Endpoints

- GET /health
  Description: Server health check endpoint.

- POST /decrypt
  Description: Initiates decryption flow.
  Request Body: { "password": "...", "encrypted_package": "..." }
  Response: { "success": true, "session_id": "..." }

- POST /otp/verify
  Description: Verifies the OTP.
  Request Body: { "session_id": "...", "otp": "..." }
  Response: Returns the decrypted plaintext on success.

---

## Project Notes
- Never commit your `.env` file to version control (ensure `.env` is listed in your `.gitignore`).
- System socket libraries (ws2_32, crypt32, wsock32) are dynamically linked on Windows builds.
- The backend does not expose an active /encrypt endpoint; local encryption is handled on the frontend using password-derived AES-GCM.

## Core Source Files
- src/controllers/decrypt_controller.c — Handles payload receiving and session setup
- src/controllers/otp_controller.c — Handles multi-factor validation
- src/services/crypto_service.c — Business logic for decryption routines
- src/crypto/aes.c — Low-level OpenSSL AES wrapper functions