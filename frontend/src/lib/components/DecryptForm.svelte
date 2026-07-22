<script>
    import { sendDecryptionRequest, verifyOtp } from '$lib/services/api.js';

    let encryptedPackage = $state("");
    let password = $state("");
    let otp = $state("");
    let sessionId = $state("");
    let decryptedMessage = $state("");
    let errorLog = $state("");
    let otpReady = $state(false);
    let isLoading = $state(false);

    async function requestOtpSession() {
        errorLog = "";
        decryptedMessage = "";
        otp = "";
        otpReady = false;
        sessionId = "";

        if (!encryptedPackage.trim() || !password.trim()) {
            errorLog = "Encrypted package and password are required.";
            return;
        }

        isLoading = true;

        try {
            const result = await sendDecryptionRequest(encryptedPackage, password);
            console.log("Decryption Response:", result);

            const retrievedSessionId = result?.session_id || result?.sessionId || result?.data?.session_id;

            if (!retrievedSessionId) {
                throw new Error("No session ID returned from backend.");
            }

            sessionId = retrievedSessionId;
            otpReady = true;
        } catch (err) {
            console.error("Error requesting OTP session:", err);
            errorLog = err.message || "Failed to request OTP session from the backend.";
        } finally {
            isLoading = false;
        }
    }

    async function verifyOtpCode() {
        errorLog = "";
        decryptedMessage = "";

        if (!sessionId.trim()) {
            errorLog = "No OTP session is available. Start by requesting decryption.";
            return;
        }

        if (!otp.trim()) {
            errorLog = "OTP cannot be blank.";
            return;
        }

        isLoading = true;

        try {
            const result = await verifyOtp(sessionId, otp);
            console.log("Verification Response:", result);

            // Explicitly inclusion of "plaintext" from C controller
            const outputText = result?.plaintext || result?.text || result?.decrypted_text || result?.message || result?.data;

            if (!outputText) {
                throw new Error("Decryption succeeded but no text payload was received.");
            }

            decryptedMessage = outputText;
        } catch (err) {
            console.error("Error verifying OTP:", err);
            errorLog = err.message || "OTP verification failed.";
        } finally {
            isLoading = false;
        }
    }
</script>
<div class="form">
    {#if errorLog}
        <div class="error-banner">{errorLog}</div>
    {/if}

    <label for="encrypted-package">Encrypted Package</label>
    <textarea
        id="encrypted-package"
        bind:value={encryptedPackage}
        placeholder="Paste the encrypted package here..."
        disabled={isLoading}
    ></textarea>

    <label for="password">Password</label>
    <input
        id="password"
        type="password"
        bind:value={password}
        placeholder="Enter the password used during encryption"
        disabled={isLoading}
    />

    <button type="button" onclick={requestOtpSession} disabled={isLoading}>
        {isLoading && !otpReady ? "Requesting OTP..." : "Request OTP session"}
    </button>

    {#if otpReady}
        <div class="otp-note">
            OTP session created. Enter the code below, then verify to unlock the message.
        </div>

        <label for="otp">OTP</label>
        <input
            id="otp"
            type="text"
            bind:value={otp}
            placeholder="Enter the OTP received from the server"
            disabled={isLoading}
        />

        <button type="button" onclick={verifyOtpCode} disabled={isLoading}>
            {isLoading ? "Verifying..." : "Verify OTP and decrypt"}
        </button>
    {/if}

    <label for="decrypted-message">Decrypted Message</label>
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
    textarea,
    button {
        padding: 0.6rem;
        font-size: 1rem;
    }

    button {
        cursor: pointer;
    }

    button:disabled {
        opacity: 0.6;
        cursor: not-allowed;
    }

    .error-banner {
        color: #721c24;
        background-color: #f8d7da;
        border: 1px solid #f5c6cb;
        padding: 0.6rem;
        border-radius: 4px;
    }

    .otp-note {
        padding: 0.5rem;
        background-color: #e2f0d9;
        color: #2b542c;
        border: 1px solid #d6e9c6;
        border-radius: 4px;
    }
</style>