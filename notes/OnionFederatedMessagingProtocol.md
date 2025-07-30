# Onion Federated Messaging Protocol (OFMP) v0.1

## Overview

A decentralized, onion-routed, identity-preserving messaging protocol supporting text, voice, file sharing, and future extensibility to multiparty sessions (e.g., group chats or conferences).

## Goals

* Strong end-to-end encryption (E2EE)
* Metadata minimization
* Onion transport (Tor-only)
* Asynchronous and synchronous communication
* Extendable for voice, file sharing, and conferencing

## Terminology

* **Identity Key**: Ed25519 public/private key pair (per user)
* **Session Key**: Ephemeral X25519 ECDH key used per session
* **Onion Server**: User-operated or trusted relay using Tor hidden service
* **Message Envelope**: JSON-wrapped encrypted message with metadata

## Identity and Addressing

```json
{
  "user_id": "base32(SHA256(ed25519_pubkey))",
  "onion_address": "r4xqozz5mjxw5mrl...onion",
  "identity_key": "base64(ed25519_pubkey)",
  "server_key": "base64(ed25519_pubkey of relay server)",
  "timestamp": 1680000000
}
```

## Message Envelope

```json
{
  "version": 1,
  "from": "user_id",
  "to": "user_id",
  "timestamp": 1680000000,
  "type": "text|voice|file|control",
  "session": {
    "ephemeral_pub": "base64(X25519_pubkey)",
    "ratchet": "optional state"
  },
  "payload": "base64(encrypted_message_blob)",
  "signature": "base64(ed25519_sig(payload))"
}
```

## Encryption Flow

1. Establish shared secret with recipient via X25519 + HKDF
2. Encrypt payload with AES-256-GCM using derived key
3. Sign payload with sender's Ed25519 key
4. Wrap in JSON envelope and transmit over Tor

## Message Types

### Text

```json
{
  "type": "text",
  "content": "UTF-8 message string"
}
```

### Voice (Push-to-talk style)

```json
{
  "type": "voice",
  "codec": "opus",
  "rate": 16000,
  "frame_duration": 20,
  "data": ["base64(frame1)", "base64(frame2)", ...]
}
```

### File

```json
{
  "type": "file",
  "filename": "example.pdf",
  "mimetype": "application/pdf",
  "size": 204800,
  "data": ["base64(chunk1)", "base64(chunk2)", ...]
}
```

### Control

```json
{
  "type": "control",
  "command": "handshake|group_invite|group_leave|ack|ping",
  "data": {}
}
```

## Inbox and Retrieval

Each onion server stores received envelopes encrypted for the intended recipient. Inbox listing is requestable via:

```json
{
  "action": "list_messages",
  "since": 1680000000
}
```

And messages are fetched via:

```json
{
  "action": "get_message",
  "id": "uuid or hash of message"
}
```

## Conference Extensions (Future)

Group sessions require:

* Group key negotiation (pre-shared or multiparty DH)
* Consent via signed control messages
* Designated group session host (rotatable)

## Transport Layer

* All traffic transmitted via Tor v3 onion services
* Onion addresses exchanged out-of-band or piggybacked in signed invites
* Optional cover traffic for timing obfuscation

## Security Notes

* Use deterministic Ed25519 signatures
* Avoid message correlation via timing
* Rotate ephemeral session keys frequently
* Apply forward secrecy via Double Ratchet (optional phase 2)

## Versioning

* This is OFMP v0.1 — future extensions to support ratchets, mixnets, and onion mailboxes

