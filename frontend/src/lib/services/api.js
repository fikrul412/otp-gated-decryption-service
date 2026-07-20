const API_BASE_URL = 'http://localhost:3000';

/**
 * Sends a structured payload to the C backend for encryption.
 * @param {string} methodName Name of the selected method (e.g., "End-to-End Encryption (E2EE)")
 * @param {string} plainText The raw message content
 * @param {string} recipientType Target medium ("Email" or "Phone Number")
 * @param {string} recipient Actual endpoint value
 * @returns {Promise<{encrypted_data: string, generated_key: string}>}
 */
export async function sendEncryptionRequest(methodName, plainText, recipientType, recipient) {
    try {
        const response = await fetch(`${API_BASE_URL}/encrypt`, {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json'
            },
            body: JSON.stringify({
                method: methodName,
                message: plainText,
                recipient_type: recipientType,
                recipient: recipient
            })
        });

        if (!response.ok) {
            let errText = `Server status returned: ${response.status}`;
            try {
                const errJson = await response.json();
                if (errJson && (errJson.error || errJson.message)) {
                    errText += ` - ${errJson.error || errJson.message}`;
                }
            } catch (e) {
                // ignore JSON parse errors, keep plain status
            }
            throw new Error(errText);
        }

        return await response.json();
    } catch (error) {
        console.error('Network execution failure:', error);
        throw error;
    }
}

/**
 * Sends encrypted data to the C backend for decryption.
 * @param {string} methodName Name of the selected method
 * @param {string} encryptedData The base64-encoded encrypted payload
 * @param {string} otp One-time password for verification
 * @param {string} encryptionKey The base64-encoded key+IV token
 * @returns {Promise<{message: string}>}
 */
export async function sendDecryptionRequest(methodName, encryptedData, otp, encryptionKey) {
    try {
        const response = await fetch(`${API_BASE_URL}/decrypt`, {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json'
            },
            body: JSON.stringify({
                method: methodName,
                encrypted_data: encryptedData,
                otp: otp,
                encryption_key: encryptionKey
            })
        });

        if (!response.ok) {
            let errText = `Server status returned: ${response.status}`;
            try {
                const errJson = await response.json();
                if (errJson && (errJson.error || errJson.message)) {
                    errText += ` - ${errJson.error || errJson.message}`;
                }
            } catch (e) {
                // ignore
            }
            throw new Error(errText);
        }

        return await response.json();
    } catch (error) {
        console.error('Decryption request failed:', error);
        throw error;
    }
}