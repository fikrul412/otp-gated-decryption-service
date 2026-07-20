Markdown
# API Testing Documentation

This document outlines the endpoints, HTTP methods, and payload structures for testing the Encrypt-Decrypt API service.

---

## 1. Health Check

Verifies if the Mongoose web server loop is up and running.

* **URL:** `http://localhost:3000/health`
* **Method:** `GET`
* **Headers:** None required

### Example Request
```http
GET /health HTTP/1.1
Host: localhost:3000
```

## 2. Encrypt Message
Submits a plaintext message to be processed by the AES-256-CBC service layer.

* **URL:** http://localhost:3000/encrypt
* **Method:** POST
* **Headers:**
  * Content-Type: application/json

### Request Body (Raw JSON)
```json
{
  "method": "End-to-End Encryption (E2EE)",
  "message": "Hello World",
  "recipient_type": "Email",
  "recipient": "alice@example.com"
}
```
## 3. Decrypt Message
Submits the Base64 ciphertext (encrypted_data) along with the verification token and OTP passcode to recover the original message.

* **URL:** http://localhost:3000/decrypt
* **Method:** POST
* **Headers:**
  * Content-Type: application/json

### Request Body (Raw JSON)
```json
{
  "method": "End-to-End Encryption (E2EE)",
  "encrypted_data": "F1e/10BFD3SSfQ2jwrakQKu4Z0nkvmUaLO1bZ7vl6AgPtYDOzt4f84MdaFUI3puswxpp1CHs3z3nULCxS+FMC08bRFrF3kxl1BTeSk9LKhBKrSpj6LEBpPQ4Tn+KBD8iOnlrbYAXUe+V3mQgFJdJ7Vh9asNUJtGq830Lk89Locs=",
  "otp": "123456",
  "encryption_key": "wMtnpW+TRvSDVSIg0pqeDA/8PrDLkAMY/bW/oKeFJy5631/nZmIEFBc3Y4e6ePfr"
}
```

## 4. Send OTP
Triggers a one-time passcode sequence to verification targets before decryption execution.

* **URL:** http://localhost:3000/otp/send
* **Method:** POST
* **Headers:**
  * Content-Type: application/json

### Request Body (Raw JSON)
```json
{
  "recipient_type": "Email",
  "recipient": "alice@example.com"
}
```