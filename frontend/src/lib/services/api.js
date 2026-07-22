import { PUBLIC_API_BASE_URL } from '$env/static/public';

const API_BASE_URL = PUBLIC_API_BASE_URL || 'http://localhost:3000';

function bufferToBase64(buffer) {
    const bytes = new Uint8Array(buffer);
    let binary = "";
    const chunkSize = 0x8000;
    for (let i = 0; i < bytes.length; i += chunkSize) {
        binary += String.fromCharCode(...bytes.subarray(i, i + chunkSize));
    }
    return btoa(binary);
}

function base64ToArrayBuffer(base64) {
    const binary = atob(base64);
    const len = binary.length;
    const bytes = new Uint8Array(len);
    for (let i = 0; i < len; i++) {
        bytes[i] = binary.charCodeAt(i);
    }
    return bytes.buffer;
}

function stringToArrayBuffer(text) {
    return new TextEncoder().encode(text);
}

function arrayBufferToString(buffer) {
    return new TextDecoder().decode(buffer);
}

async function deriveKey(password, salt, iterations = 100000) {
    const passwordBytes = stringToArrayBuffer(password);
    const baseKey = await crypto.subtle.importKey(
        'raw',
        passwordBytes,
        'PBKDF2',
        false,
        ['deriveBits']
    );

    const bits = await crypto.subtle.deriveBits(
        {
            name: 'PBKDF2',
            salt,
            iterations,
            hash: 'SHA-256'
        },
        baseKey,
        256
    );

    return crypto.subtle.importKey(
        'raw',
        bits,
        { name: 'AES-GCM' },
        false,
        ['encrypt', 'decrypt']
    );
}

async function encryptAesGcm(plaintext, key, iv) {
    const encrypted = await crypto.subtle.encrypt(
        {
            name: 'AES-GCM',
            iv,
            tagLength: 128
        },
        key,
        stringToArrayBuffer(plaintext)
    );

    const encryptedBytes = new Uint8Array(encrypted);
    const tag = encryptedBytes.slice(encryptedBytes.length - 16);
    const ciphertext = encryptedBytes.slice(0, encryptedBytes.length - 16);
    return { ciphertext, tag };
}

async function buildEncryptedPackage(message, recipientType, recipient, password) {
    const salt = crypto.getRandomValues(new Uint8Array(16));  // 16 bytes
    const nonce = crypto.getRandomValues(new Uint8Array(12)); // 12 bytes
    const key = await deriveKey(password, salt);

    const payload = JSON.stringify({
        text: message,
        recipient_type: recipientType,
        recipient: recipient
    });

    const { ciphertext, tag } = await encryptAesGcm(payload, key, nonce);

    const totalLength = salt.length + nonce.length + tag.length + ciphertext.length;
    const combined = new Uint8Array(totalLength);

    combined.set(salt, 0);
    combined.set(nonce, 16);
    combined.set(tag, 28);
    combined.set(ciphertext, 44);

    return bufferToBase64(combined.buffer);
}

function unpackBase64Package(packedBase64) {
    if (packedBase64.trim().startsWith('{')) {
        return JSON.parse(packedBase64);
    }

    const buffer = base64ToArrayBuffer(packedBase64);
    const bytes = new Uint8Array(buffer);

    if (bytes.length < 44) {
        throw new Error("Invalid encrypted package length.");
    }

    const salt = bytes.slice(0, 16);
    const nonce = bytes.slice(16, 28);
    const tag = bytes.slice(28, 44);
    const ciphertext = bytes.slice(44);

    return {
        salt: bufferToBase64(salt.buffer),
        nonce: bufferToBase64(nonce.buffer),
        ciphertext: bufferToBase64(ciphertext.buffer),
        authentication_tag: bufferToBase64(tag.buffer)
    };
}

async function extractErrorMessage(response) {
    try {
        const errJson = await response.json();
        if (errJson && (errJson.error || errJson.message)) {
            return errJson.error || errJson.message;
        }
    } catch (e) {
    }
    return `Server status returned: ${response.status}`;
}

/**
 * @param {string} plainText 
 * @param {string} recipientType 
 * @param {string} recipient 
 * @param {string} password 
 * @returns {Promise<{encrypted_package: string}>}
 */
export async function sendEncryptionRequest(plainText, recipientType, recipient, password) {
    try {
        const encrypted_package = await buildEncryptedPackage(plainText, recipientType, recipient, password);
        return { encrypted_package };
    } catch (error) {
        console.error('Local encryption failure:', error);
        throw error;
    }
}

/**
 * @param {string|object} encryptedPackage 
 * @param {string} password 
 * @returns {Promise<{session_id: string}>}
 */
export async function sendDecryptionRequest(encryptedPackage, password) {
    try {
        const parsedPackage = typeof encryptedPackage === 'string'
            ? unpackBase64Package(encryptedPackage)
            : encryptedPackage;

        const response = await fetch(`${API_BASE_URL}/decrypt`, {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json'
            },
            body: JSON.stringify({
                password: password,
                encrypted_package: parsedPackage
            })
        });

        if (!response.ok) {
            const errorMessage = await extractErrorMessage(response);
            throw new Error(errorMessage);
        }

        return await response.json();
    } catch (error) {
        console.error('Decryption request failed:', error);
        throw error;
    }
}

/**
 * @param {string} sessionId 
 * @param {string} otp 
 * @returns {Promise<{plaintext: string}>}
 */
export async function verifyOtp(sessionId, otp) {
    try {
        const response = await fetch(`${API_BASE_URL}/otp`, {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json'
            },
            body: JSON.stringify({
                session_id: sessionId,
                otp: otp
            })
        });

        if (!response.ok) {
            const errorMessage = await extractErrorMessage(response);
            throw new Error(errorMessage);
        }

        return await response.json();
    } catch (error) {
        console.error('OTP verification failed:', error);
        throw error;
    }
}