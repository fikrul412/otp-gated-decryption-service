# Frontend — Svelte Local Encryption UI

The frontend performs local password-based encryption using Web Crypto (AES-256-GCM) and drives the backend decryption and OTP verification flow.

## Tech Stack
- Framework: Svelte 5 / SvelteKit
- Build Tool: Vite
- Runtime: Node.js (v18+)
- Package Manager: npm

---

## Directory Structure
```
frontend/
├── .svelte-kit/           # Generated SvelteKit build & dev output
├── node_modules/          # Installed npm dependencies
├── static/                # Static public assets
└── src/
    ├── lib/               # Shared logic and reusable components
    │   ├── assets/        # Styles, images, and visual assets
    │   ├── components/    # Svelte UI components (e.g., forms, buttons)
    │   └── services/      # Client API handlers & encryption routines (api.js)
    └── routes/            # SvelteKit page routes
        └── decrypt/       # Decryption & OTP flow view page

```

## What It Does

1. Local Encryption:
   - Encrypts the client-side payload using password-derived AES-256-GCM (Web Crypto API).
   - Packs binary outputs into a standard format: [16B Salt][12B Nonce][16B Tag][Ciphertext].
   - Encodes the binary package as Base64 for clean payload transmission.

2. Backend Decryption Request:
   - Sends the packed payload to POST /decrypt on the C backend.
   - On success, stores the returned session_id for two-factor verification.

3. OTP Verification:
   - Captures the user's OTP input.
   - Sends session_id and OTP to POST /otp/verify to receive the decrypted response.

---

## Environment Setup (.env)

Create a .env file in the root of the frontend/ directory:
```
PUBLIC_API_BASE_URL=http://localhost:3000
```

## Installation & Setup

1. Navigate to the frontend directory:
```
     cd frontend
```
2. Install dependencies:
```
     npm install
```
---

## Running the Development Server

Start the Vite development server:
```
  npm run dev
```
By default, the application will be accessible at http://localhost:5173.

---

## Building for Production

To create an optimized production build:
```
  npm run build
```
To preview the built application locally:
```
  npm run preview
```
---

## Key Files of Interest

- src/routes/decrypt/ — The primary UI page routing for the decryption flow.
- src/lib/services/api.js — Unpacks Base64 payload packages, handles Web Crypto calls, and interacts with backend API endpoints.
- src/lib/components/ — UI forms for file upload, password input, and OTP entry.

---

## Project Notes
- The frontend operates entirely through local encryption. There is no server-side /encrypt route called.
- Make sure your backend C service (Mongoose API) is running on port 3000 before attempting decryption or OTP submissions.