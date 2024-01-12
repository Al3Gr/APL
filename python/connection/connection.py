import base64

# Questa classe astrae le operazioni di ricezione dell'immagine e trasmissione della relativa risposta
# attraverso le socket
class Connection:

    # setto i parametri della connessione
    def __init__(self, conn, addr, id):
        self.conn = conn
        self.addr = addr
        self.id = id

    # Ricevo l'immagine attraverso la socket e la salva su RAM in modo da permettere al tagger di poter analizzare
    # l'immagine ricevuta e restituire i relativi tag
    def receive_image(self):
        print(f'Connected with {self.addr}')
        # In particolare, prima ricevo la dimensione dell'immagine in modo tale da essere
        # sicuro di ricevere tutti i suoi byte, senza lasciarne alcuno 'indietro'
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

    # trasmetto i tag al server
    # essi vengono opportunamenti formattati in una stringa in modo da permettere al server di poter analizzare la
    # risposta in modo corretto
    def send_response(self, tags):
        print("Send response to client")
        response = ""
        for t in tags:
            response = response + t + "-"
        self.conn.sendall(response.encode())
