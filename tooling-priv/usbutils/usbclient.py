#!/usr/bin/env python3

import argparse
import socket
import subprocess
import os

SERVER = '172.24.57.165'
PORT = 12345

FINISH_MAGIC = b'FINISHMAGICFINISHMAGICFINISHMAGIC92944838'

def main():

    parser = argparse.ArgumentParser()
    parser.add_argument("-f", "--file", help="elf file to flash")
    parser.add_argument("-a", "--address", help="usb address to use")
    parser.add_argument("command", help="command to run")

    args = parser.parse_args()

    if args.command == 'flash':
        parse_flash(args.file, args.address)

def parse_flash(file_path, address):

    if not file_path:
        print('error: --file not set')
        return

    if int(address) not in range(0, 9):
        print('bad address')
        return

    # Open the file in binary mode
    with open(file_path, 'rb') as file:
        file_data = file.read()

    # Create a TCP/IP socket
    client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    try:
        # Connect to the server
        client_socket.connect((SERVER, PORT))

        command = b'FLSH ' + address.encode() + b' ' + file_data

        # Send the file data
        client_socket.sendall(command)
        client_socket.sendall(FINISH_MAGIC)
        print('sent')

        # Receive echoed data from the server
        response = client_socket.recv(4096)
        print(response.decode())

        print(f'Flash success! run `load_serial {address}` to initialize `/dev/ttyPICO{address}`')
    finally:
        # Close the client socket
        client_socket.close()
        exit(address)

if __name__ == '__main__':
    try:
        main()
    except Exception as e:
        print(e)
        exit(-1)