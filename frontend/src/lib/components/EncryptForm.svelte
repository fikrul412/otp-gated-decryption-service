<script>
    // Import the utility from your separate file
    import { sendEncryptionRequest } from '$lib/services/api.js';

    let message = $state("");
    let recipientType = $state("Email");
    let recipient = $state("");

    let generatedKey = $state("");
    let encryptedData = $state("");
    let errorLog = $state("");

    const methods = [
        {
            name: "Key in DB",
            requireRecipient: true,
            showGeneratedKey: false
        },
        {
            name: "End-to-End Encryption (E2EE)",
            requireRecipient: true,
            showGeneratedKey: true
        }
    ];

    let method = $state(methods[0]);

    // The state updating function remains minimal and isolated
    async function handleEncrypt() {
        // 1. Reset our error log and state fields before the round trip starts
        errorLog = "";
        encryptedData = "";
        generatedKey = "";
        
        if (!message.trim()) {
            errorLog = "Message payload cannot be blank.";
            return;
        }

        try {
            // 2. Await the response from our isolated API handler
            const data = await sendEncryptionRequest(
                method.name,
                message,
                recipientType,
                recipient
            );
            
            // 3. Handle a successful response payload
            encryptedData = data.encrypted_data;
            
            if (method.showGeneratedKey) {
                generatedKey = data.generated_key;
            } else {
                generatedKey = "";
            }
        } catch (err) {
            // 4. Handle a network/decryption response failure cleanly
            errorLog = "Could not communicate with the backend encryption engine.";
            encryptedData = "CRITICAL ERROR: Check terminal logs.";
            generatedKey = "CRITICAL ERROR: Missing token.";
        }
    }

</script>

<div class="form">
    {#if errorLog}
        <div class="error-banner">{errorLog}</div>
    {/if}

    <label for="method">Encryption Method</label>
    <select id="method" bind:value={method}>
        {#each methods as item}
            <option value={item}>{item.name}</option>
        {/each}
    </select>

    <label for="message">Message</label>
    <textarea
        id="message"
        bind:value={message}
        placeholder="Enter your message..."
    ></textarea>

    {#if method.requireRecipient}
        <label for="recipient-type">Send OTP To</label>
        <select id="recipient-type" bind:value={recipientType}>
            <option>Email</option>
            <option>Phone Number</option>
        </select>

        <input
            type="text"
            bind:value={recipient}
            placeholder={recipientType === "Email" ? "Enter email..." : "Enter phone number..."}
        />
    {/if}

    <!-- Target your new async runner here -->
    <button onclick={handleEncrypt}>
        Encrypt via C Backend
    </button>

    {#if method.showGeneratedKey}
        <label for="generated-key">Generated Encryption Key</label>
        <textarea
            id="generated-key"
            readonly
            bind:value={generatedKey}
            placeholder="Generated key will appear here..."
        ></textarea>
    {/if}

    <label for="encrypted-data">Encrypted Data</label>
    <textarea
        id="encrypted-data"
        readonly
        bind:value={encryptedData}
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