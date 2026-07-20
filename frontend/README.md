# Frontend — Svelte App

Brief
- Small Svelte + Vite frontend used to call the C backend's encrypt/decrypt endpoints. The UI contains two forms: encryption (produce `encrypted_data` + `generated_key`) and decryption (consume both to return plaintext message).

Tech stack
- Svelte (Vite)
- Node.js (use a modern LTS, e.g., Node 18+)
- npm

Install & run
1. Open a terminal in `frontend/`.
2. Install dependencies:

```bash
npm install
```

3. Start dev server:

```bash
npm run dev
```

Files of interest
- `src/lib/components/EncryptForm.svelte` — encryption UI and wiring
- `src/lib/components/DecryptForm.svelte` — decryption UI and wiring
- `src/lib/services/api.js` — network calls to the backend (`/encrypt`, `/decrypt`)

Usage
- Use the Encrypt form to create an `Encrypted Data` and `Generated Encryption Key`.
- Copy both values into the Decrypt form along with the OTP (the OTP flow is present but the backend currently treats OTP verification as a stub).

Notes
- The frontend expects the backend at `http://localhost:3000` by default (`API_BASE_URL` in `src/lib/services/api.js`). Update if your server listens on a different host/port.
# sv

Everything you need to build a Svelte project, powered by [`sv`](https://github.com/sveltejs/cli).

## Creating a project

If you're seeing this, you've probably already done this step. Congrats!

```sh
# create a new project
npx sv create my-app
```

To recreate this project with the same configuration:

```sh
# recreate this project
npx sv@0.16.3 create --template minimal --no-types --install npm frontend
```

## Developing

Once you've created a project and installed dependencies with `npm install` (or `pnpm install` or `yarn`), start a development server:

```sh
npm run dev

# or start the server and open the app in a new browser tab
npm run dev -- --open
```

## Building

To create a production version of your app:

```sh
npm run build
```

You can preview the production build with `npm run preview`.

> To deploy your app, you may need to install an [adapter](https://svelte.dev/docs/kit/adapters) for your target environment.
