import base64


class Connection():

    def __init__(self, conn, addr, id):
        self.conn = conn
        self.addr = addr
        self.id = id

    def receive_image(self):
        print(f'Connected with {self.addr}')
        size_b = self.conn.recv(4)
        size = int.from_bytes(size_b, "little")
        print(f"Image's dimension: {size}")
        with open(f"server_image_{self.id}.jpeg", "wb") as f:
            while size > 0:
                image_64 = self.conn.recv(2048)
                image_data = base64.b64decode(image_64)
                f.write(image_data)
                size = size - len(image_64)
            print("Server image received")
        return f"server_image_{self.id}.jpeg"

    def send_response(self, tags):
        print("Send response to client")
        response = ""
        for t in tags:
            response = response + t + "-"
        
        self.conn.sendall(response.encode())
