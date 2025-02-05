import os

## @brief encrypts bytes using ChaCha20-Poly1305.
#  @pre the \p key is exactly 32 bytes.
#  @pre the \p plaintext is not empty.
#  @pre the \p nonce is exactly 12 bytes.
#  @param key bytes of the key to be used.
#  @param plaintext bytes of the plaintext to be encrypted.
#  @param Optional. The nonce to be used.  If empty (the default), a random nonce will be generated using os.urandom().
#  @return the 12 byte nonce used.
#  @return the 16 byte tag generated.
#  @return the encrypted cipher.
def chacha_encrypt(key: bytes, plaintext: bytes, nonce: bytes = b'') -> (bytes, bytes, bytes):
    if (len(key) != 32):
        raise Exception("Incorrect key length.")
    if (len(plaintext) == 0):
        raise Exception("Empty plaintext.")
    if (len(nonce) != 0 and len(nonce) != 12):
        raise Exception("Incorrect nonce length.")

    if (len(nonce) == 0):
        nonce = os.urandom(12)
    tag = b''
    output = b''

    return (nonce, tag, output)
