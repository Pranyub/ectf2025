#!/usr/bin/env python3

import socket
import threading
import tempfile
import subprocess
import time

MAX_FILE_SIZE = 20 * 1024 * 1024  # 5MB

FINISH_MAGIC = b'FINISHMAGICFINISHMAGIC'+b'FINISHMAGIC92944838'

shorthand = {
    4: '8081BA7854452112',
    5: '9CC1F3509B614BDB',
    2: '7243898FD1AE65B2',
    6: 'B3170728920898E1',
    3: 'ABE3747F7B62ED78',
    1: '8D4C20BE3FA69534',
    6: '0FA10586986596B1',
    0: '7ED67CFF1CDC2823',
    8: 'C9788CB73DC7BDD2'
}

def list_connected_picos():
    try:
        # Run the picotool command to list all connected Picos
        result = subprocess.run(['picotool', 'info', '-a'], stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)

        # Check if the command ran successfully
        if result.returncode == 0:
            output = result.stdout.strip()
            if output:
                print("Connected Pi Picos:\n")
                print(output)
            else:
                print("No Pi Picos found.")
        else:
            print(f"Error running picotool: {result.stderr}")
    except FileNotFoundError:
        print("picotool not found. Please make sure it's installed and in your PATH.")

def handle_client(client_socket):
    try:
        # Buffer to hold incoming data
        received_data = b""
        
        while FINISH_MAGIC not in received_data:
            # Receive data from the client
            chunk = client_socket.recv(4096)  # Receive in chunks of 4KB
            if not chunk:
                break  # Client disconnected

            received_data += chunk

            # If the received data exceeds the limit, close the connection
            if len(received_data) > MAX_FILE_SIZE:
                print("Received file exceeds 5MB limit. Closing connection.")
                break

        received_data = received_data[:received_data.index(FINISH_MAGIC)]

        command, data = received_data.split(b' ', 1)

        if command == b'FLSH':
            handle_flash(client_socket, data)
        elif command == b'OOCD':
            program_with_openocd(client_socket, data)
        elif command == b'SGDB':
            start_gdbserver(client_socket, data)

    except ConnectionResetError:
        print("Client forcibly closed the connection.")
    finally:
        # Close the client socket
        client_socket.close()

def handle_flash(client_socket, data):

    try:
        ser, data = data.split(b' ', 1)
        ser = shorthand[int(ser.decode(), 16)]

    except:
        client_socket.sendall(b'Error: bad address ' + ser.encode())
        return

    temp = tempfile.NamedTemporaryFile(suffix='.elf')
    temp.write(data)
    temp.flush()

    command = ['picotool', 'load', '-f', '--ser', ser, '-x', temp.name]

    result = subprocess.run(command, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
    output = result.stdout.strip()
    print(output)
    print(result.stderr)
    temp.close()

    client_socket.sendall((' '.join(command)+ ' returned:\n' + output + result.stderr.strip()).encode())

def program_with_openocd(client_socket, data):

    try:
        ser, data = data.split(b' ', 1)
        ser = shorthand[int(ser.decode(), 16)]

    except:
        client_socket.sendall(b'Error: bad address ' + ser.encode())
        return

    temp = tempfile.NamedTemporaryFile(suffix='.elf')
    temp.write(data)
    temp.flush()

    command = ['openocd', '-s', '/home/litwick/openocd/tcl', '-f', 'interface/cmsis-dap.cfg', '-f', 'target/rp2350.cfg',
    '-c', f'adapter serial {ser}; adapter speed 5000; telnet_port disabled; tcl_port disabled; gdb_port disabled; program {temp.name} verify; reset run; exit']

    result = subprocess.run(command, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
    output = result.stdout.strip()
    print(output)
    print(result.stderr)
    temp.close()

    client_socket.sendall((' '.join(command)+ ' returned:\n' + output + result.stderr.strip()).encode())

def start_gdbserver(client_socket, data):
    try:
        addr, data = data.split(b' ', 1)
        ser = shorthand[int(addr.decode(), 16)]

    except:
        client_socket.sendall(b'Error: bad address ' + ser.encode())
        return

    gdb_port = 3330 + int(addr.decode())

    command = ['openocd', '-s', '/home/litwick/openocd/tcl', '-f', 'interface/cmsis-dap.cfg', '-f', 'target/rp2350.cfg',
    '-c', f'adapter serial {ser}; adapter speed 5000; telnet_port disabled; tcl_port disabled; gdb_port {gdb_port}; bindto 0.0.0.0']

    process = subprocess.Popen(command, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)

    #client_socket.sendall(b'running: ' + ' '.join(command).encode() + b'\n')
    client_socket.sendall(f'starting gdbserver on port {gdb_port}\n\n'.encode())

    # Continuously read output from the subprocess
    try:
        while True:
            #output = process.stdout.read()  # Read line by line
            time.sleep(5)
            client_socket.sendall(b'a')

            if process.poll() is not None:  # If the process has finished
                break
    finally:
        process.terminate()

# Main server function
def start_server(host='0.0.0.0', port=12345):
    # Create a TCP/IP socket
    server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    # Allow address reuse
    server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)

    # Bind the server to the specified host and port
    server.bind((host, port))

    # Start listening for incoming connections
    server.listen(5)
    print(f"Server listening on {host}:{port}")

    while True:
        # Accept a new client connection
        client_socket, client_address = server.accept()
        print(f"Accepted connection from {client_address}")

        # Start a new thread to handle the client
        client_handler = threading.Thread(target=handle_client, args=(client_socket,))
        client_handler.start()

# Run the server
if __name__ == "__main__":
    list_connected_picos()
    start_server()
