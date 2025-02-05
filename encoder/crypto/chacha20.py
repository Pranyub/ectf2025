from Crypto.Cipher import ChaCha20_Poly1305
from Crypto.Random import get_random_bytes

## @brief encrypts bytes using ChaCha20-Poly1305.
#  @pre the \p key is exactly 32 bytes.
#  @pre the \p plaintext is not empty.
#  @pre the \p nonce is exactly 12 bytes.
#  @param key bytes of the key to be used.
#  @param plaintext bytes of the plaintext to be encrypted.
#  @param ad Optional. the additional data
#  @param nonce Optional. The nonce to be used.  If empty (the default), a random nonce will be generated using os.urandom().
#  @return the 12 byte nonce used.
#  @return the 16 byte tag generated.
#  @return the encrypted cipher.
def chacha_encrypt(key: bytes, plaintext: bytes, ad: bytes = b'', nonce: bytes = b'') -> (bytes, bytes, bytes):
    if (len(key) != 32):
        raise Exception(f"Incorrect key length, expected 32 bytes, got {len(key)} bytes.")
    if (len(plaintext) == 0):
        raise Exception("Empty plaintext.")
    if (len(nonce) != 0 and len(nonce) != 12):
        raise Exception("Incorrect nonce length.")

    if (len(nonce) == 0):
        nonce = get_random_bytes(12)
    tag = b''
    output = b''

    return (nonce, tag, output)
