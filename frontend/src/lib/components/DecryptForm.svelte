<script>
    import { sendDecryptionRequest } from '$lib/services/api.js';

    let encryptedData = $state("");
    let otp = $state("");
    let encryptionKey = $state("");
    let decryptedMessage = $state("");
    let errorLog = $state("");

    const methods = [
        {
            name: "Key in DB",
            requireOtp: true,
            requireKey: false
        },

        {
            name: "End-to-End Encryption (E2EE)",
            requireOtp: true,
            requireKey: true
        }
    ];

    let method = $state(methods[0]);

    function sendOtp() {
        // TODO:
        // Send encrypted data to backend.
        // Backend identifies recipient and sends OTP.
    }

    async function decrypt() {
        errorLog = "";
        decryptedMessage = "";

        if (!encryptedData.trim()) {
            errorLog = "Encrypted data cannot be blank.";
            return;
        }

        if (!encryptionKey.trim()) {
            errorLog = "Encryption key cannot be blank.";
            return;
        }

        if (!otp.trim()) {
            errorLog = "OTP cannot be blank.";
            return;
        }

        try {
            const data = await sendDecryptionRequest(
                method.name,
                encryptedData,
                otp,
                encryptionKey
            );

            decryptedMessage = data.message;
        } catch (err) {
            errorLog = "Could not communicate with the backend decryption engine.";
            decryptedMessage = "CRITICAL ERROR: Check terminal logs.";
        }
    }
</script>

<div class="form">
    {#if errorLog}
        <div class="error-banner">{errorLog}</div>
    {/if}

    <label for="method">
        Decryption Method
    </label>

    <select
        id="method"
        bind:value={method}
    >
        {#each methods as item}
            <option value={item}>
                {item.name}
            </option>
        {/each}
    </select>


    <label for="encrypted-data">
        Encrypted Data
    </label>

    <textarea
        id="encrypted-data"
        bind:value={encryptedData}
        placeholder="Paste encrypted data..."
    ></textarea>


    {#if method.requireOtp}

        <button
            type="button"
            onclick={sendOtp}
        >
            Send OTP
        </button>

        <label for="otp">
            OTP
        </label>

        <input
            id="otp"
            type="text"
            bind:value={otp}
            placeholder="Enter the OTP..."
        />

    {/if}


    {#if method.requireKey}

        <label for="key">
            Encryption Key
        </label>

        <textarea
            id="key"
            bind:value={encryptionKey}
            placeholder="Paste the encryption key..."
        ></textarea>

    {/if}


    <button onclick={decrypt}>
        Decrypt
    </button>


    <label for="decrypted-message">
        Decrypted Message
    </label>

    <textarea
        id="decrypted-message"
        readonly
        bind:value={decryptedMessage}
        placeholder="Decrypted message will appear here..."
    ></textarea>
</div>

<style>
    .form {
        display: flex;
        flex-direction: column;
        gap: 0.8rem;
        max-width: 600px;
    }

    textarea {
        min-height: 120px;
        resize: vertical;
    }

    input,
    select,
    textarea,
    button {
        padding: 0.6rem;
        font-size: 1rem;
    }

    button {
        cursor: pointer;
    }

    .error-banner {
        color: #721c24;
        background-color: #f8d7da;
        border: 1px solid #f5c6cb;
        padding: 0.6rem;
        border-radius: 4px;
    }
</style>