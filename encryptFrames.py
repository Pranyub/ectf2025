import hashlib
from cryptography.hazmat.primitives.ciphers.aead import ChaCha20Poly1305
import os

TOTAL_FRAMES = 3

def encryptFrame(fNum, key, frame_data):
    frameKey = calcKey(fNum, key)

    nonce = os.urandom(12)
    cipher = ChaCha20Poly1305(frameKey)
    ciphertext = cipher.encrypt(nonce, frame_data, None)
    return ciphertext

def calcKey(fNum, currKey):
    low = 0
    high = TOTAL_FRAMES-1
    while high - low > 1:
        mid = low + (high - low) // 2
        if fNum <= mid:
            currKey = hash(currKey, low)
            high = mid
        else:
            currKey = hash(currKey, mid + 1)
            low = mid + 1
    return currKey

def hash(currKey, fNum):
    #frame_bytes = str(fNum).encode('utf-8').rjust(64, b'0')
    if fNum == 0:
        frame_bytes = b'\x00'  # Ensure 0 is represented at least with 1 byte
    else:
        frame_bytes = n.to_bytes((n.bit_length() + 7) // 8, byteorder='big')
    data = currKey + frame_bytes

    hashable = data + b"\0" * (((int(len(data) / 64) + 1) * 64)-len(data))

    hash_result = hashlib.sha256(hashable).digest()
    return hash_result

def main():
    key = bytes.fromhex("02a943ba302de0112c175a6f9e104bcbf4dd6bc9deff839699da8a383f976284")
    magic = b'FRA'
    channelNum = b'\x05'
    frames = ["apple", "banana", "grape"]
    for i in range (TOTAL_FRAMES):
        #print(str(i) + ": ", encryptFrame(i, key, frames[i].encode('utf-8').rjust(256, b'0')).hex())
        encFrameData = encryptFrame(i, key, frames[i].encode('utf-8').rjust(256, b'\x00'))
        #print(type(encFrameData))
        frame = magic + channelNum + i.to_bytes(8, byteorder='big') + encFrameData
        print(frame.hex())
    
main()
