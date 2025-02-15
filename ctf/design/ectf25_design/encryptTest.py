import hashlib
from cryptography.hazmat.primitives.ciphers.aead import ChaCha20Poly1305
import os

TOTAL_FRAMES = 3

def encryptFrame(fNum, key, frame_data, channel_num):
    frameKey = calcKey(fNum, key)

    nonce = os.urandom(12)
    # nonce = b'\xc3\x26\x6c\x7b\xe8\x14\x02\x3c\xae\x9b\x8e\x2c'
    ad = b''
    ad += channel_num
    ad += fNum.to_bytes(8, 'big')
    ad += nonce
    cipher = ChaCha20Poly1305(frameKey)
    ciphertext = cipher.encrypt(nonce, frame_data, ad)
    # print(f"\n\nAdditional data: {ad.hex()}\nPlaintext frame: {frame_data.hex()}\nCipher text: {ciphertext[:-16].hex()}\nKey: {frameKey.hex()}\nNonce: {nonce.hex()}\nTag: {ciphertext[-16:].hex()}\n\n")
    return ciphertext, ad

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
    frame_bytes = fNum.to_bytes(64, byteorder='big')
    data = currKey + frame_bytes
    hash_result = hashlib.sha256(data).digest()
    return hash_result

def main():
    key = bytes.fromhex("02a943ba302de0112c175a6f9e104bcbf4dd6bc9deff839699da8a383f976284")
    channelNum = b'\x05'
    frames = [b'apple\0', b'banana\0', b'grape\0']
    for i in range (TOTAL_FRAMES):
        #print(str(i) + ": ", encryptFrame(i, key, frames[i].encode('utf-8').rjust(256, b'0')).hex())
        encFrameData, ad = encryptFrame(i, key, frames[i].rjust(256, b'\x00'), channelNum)
        #print(type(encFrameData))
        frame = ad + encFrameData
        print(frame.hex(), end="\n\n")
    
main()
