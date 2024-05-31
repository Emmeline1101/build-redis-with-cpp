# import socket
# import time

# def send_command(sock, command):
#     print(f"Sending: {command}")
#     sock.sendall(f"{command}\r\n".encode())
#     # Wait for the response
#     time.sleep(0.5)
#     response = sock.recv(4096).decode().strip()
#     print(f"Received: {response}")
#     return response

# def main():
#     host, port = 'localhost', 1234
#     sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
#     sock.connect((host, port))

#     try:
#         # Test SET command
#         response = send_command(sock, "set key1 hello")
#         assert response == 'OK', "SET command did not return OK"

#         # Test GET command
#         response = send_command(sock, "get key1")
#         assert response == 'hello', "GET command did not return 'hello'"

#         # Test DEL command
#         response = send_command(sock, "del key1")
#         assert response == '1', "DEL command did not return '1'"

#         # Test GET for non-existing key (after deletion)
#         response = send_command(sock, "get key1")
#         assert response == '(nil)', "GET command for deleted key did not return '(nil)'"

#     finally:
#         sock.close()

# if __name__ == '__main__':
#     main()

import socket

def main():
    host = 'localhost'
    port = 1234
    buffer_size = 1024
    
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
        sock.connect((host, port))
        print("Connected to server successfully.")
        
        # Send SET command
        set_command = "set k v\n"
        print(f"Sending: {set_command.strip()}")
        sock.sendall(set_command.encode('utf-8'))
        response = sock.recv(buffer_size).decode('utf-8')
        print(f"Received: {response.strip()}")

        # Send GET command
        get_command = "get k\n"
        print(f"Sending: {get_command.strip()}")
        sock.sendall(get_command.encode('utf-8'))
        response = sock.recv(buffer_size).decode('utf-8')
        print(f"Received: {response.strip()}")

        # Send DEL command
        del_command = "del k\n"
        print(f"Sending: {del_command.strip()}")
        sock.sendall(del_command.encode('utf-8'))
        response = sock.recv(buffer_size).decode('utf-8')
        print(f"Received: {response.strip()}")

        # Close the connection
        sock.close()

if __name__ == "__main__":
    main()
