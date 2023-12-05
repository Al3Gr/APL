import socket
import os

HOST = "127.0.0.1"
PORT = 10001

size = os.path.getsize(
    "/Users/alessandrogravagno/Desktop/Python/test/download.jpeg")

size_b = (size).to_bytes(length=10, byteorder="big")

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    with open("download.jpeg", "rb") as f:
        s.connect((HOST, PORT))
        s.sendall(size_b)
        image_data = f.read(2048)
        while image_data:
            s.send(image_data)
            image_data = f.read(2048)
        s.send(b'')

        response = s.recv(1024)
        print(response.decode())
