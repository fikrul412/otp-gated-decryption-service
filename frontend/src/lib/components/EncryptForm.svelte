<script>
    import { sendEncryptionRequest } from '$lib/services/api.js';

    let message = $state("");
    let recipientType = $state("Email");
    let recipient = $state("");
    let password = $state("");

    let encryptedPackage = $state("");
    let errorLog = $state("");

    async function handleEncrypt() {
        errorLog = "";
        encryptedPackage = "";

        if (!message.trim() || !password.trim()) {
            errorLog = "Message and password are required for encryption.";
            return;
        }

        if (!recipient.trim()) {
            errorLog = "Recipient information is required for OTP delivery.";
            return;
        }

        try {
            const data = await sendEncryptionRequest(
                message,
                recipientType,
                recipient,
                password
            );

            encryptedPackage = data.encrypted_package || "";
        } catch (err) {
            errorLog = err.message || "Could not communicate with the backend encryption engine.";
        }
    }

</script>

<div class="form">
    {#if errorLog}
        <div class="error-banner">{errorLog}</div>
    {/if}

    <label for="message">Message</label>
    <textarea
        id="message"
        bind:value={message}
        placeholder="Enter your message..."
    ></textarea>

    <label for="password">Password</label>
    <input
        id="password"
        type="password"
        bind:value={password}
        placeholder="Enter a password used for PBKDF2 key derivation"
    />

    {#if true}
        <label for="recipient-type">Send OTP To</label>
        <select id="recipient-type" bind:value={recipientType}>
            <option>Email</option>
        </select>

        <input
            type="text"
            bind:value={recipient}
            placeholder={recipientType === "Email" ? "Enter email..." : "Enter phone number..."}
        />
    {/if}

    <button onclick={handleEncrypt}>
        Encrypt and prepare package
    </button>
    <p class="note">
        This form encrypts the payload locally using your password and prepares the encrypted package for backend OTP-based decryption.
    </p>

    <label for="encrypted-data">Encrypted Data</label>
    <textarea
        id="encrypted-data"
        readonly
        bind:value={encryptedPackage}
        placeholder="Encrypted data will appear here..."
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
    input, select, textarea, button {
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