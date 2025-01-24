#!/usr/bin/python3
# timing using the serial messages

import serial
import time

TTY_PATH = "/dev/ttyACM0"

com = serial.Serial(TTY_PATH, 115200, timeout=None)
try:
    # MD5
    com.read_until(b"\nTesting MD5.\n")
    t0 = time.time_ns()
    com.read_until(b" MD5 hashes computed in ")
    t1 = time.time_ns()
    print(f"MD5 completed in {(t1-t0) / 1e3} ")

    # SHA-1
    com.read_until(b"\nTesting SHA-1.\n")
    t0 = time.time_ns()
    com.read_until(b" SHA-1 hashes computed in ")
    t1 = time.time_ns()
    print(f"SHA-1 completed in {(t1-t0) / 1e3} ")

    # SHA-256
    com.read_until(b"\nTesting SHA-256.\n")
    t0 = time.time_ns()
    com.read_until(b" SHA-256 hashes computed in ")
    t1 = time.time_ns()
    print(f"SHA-256 completed in {(t1-t0) / 1e3} ")

    # BLAKE2s
    com.read_until(b"\nTesting BLAKE2s.\n")
    t0 = time.time_ns()
    com.read_until(b" BLAKE2s hashes computed in ")
    t1 = time.time_ns()
    print(f"BLAKE2s completed in {(t1-t0) / 1e3} ")
except KeyboardInterrupt:
    print("Ending connection...")
    com.close()

print("Done.")
