from cryptography.hazmat.primitives import hashes
from cryptography.hazmat.primitives.asymmetric import ec
from cryptography.hazmat.primitives.asymmetric import utils
from cryptography.hazmat.primitives.serialization import Encoding, PublicFormat

def generateKeys():
    keys = ec.generate_private_key(ec.SECP256K1())
    print(f"Public key: {keys.public_key().public_bytes(Encoding.PEM, PublicFormat.SubjectPublicKeyInfo)}")
    return keys

# Takes data then hashes to sign
def signHash(data: bytes, private_key: ec.EllipticCurvePrivateKey):
    hash_func = hashes.SHA256()
    hasher = hashes.Hash(hash_func)
    hasher.update(data)
    digest = hasher.finalize()
    sig = private_key.sign(digest, ec.ECDSA(utils.Prehashed(hash_func)))
    print(f"Signature: {sig.hex()}")
    return sig

def main():
    keys = generateKeys()
    data = b'Im seeing signs...\0'
    signature = signHash(data, keys)

main()
