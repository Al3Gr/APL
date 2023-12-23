import socket
import configparser
import threading
import os
from tagger.tagger import Tagger
from connection.connection import Connection


def tag_image(conn, addr, n_tags):
    id = threading.get_ident()
    with conn:
        tagger = Tagger(n_tags)
        connection = Connection(conn, addr, id)
        image = connection.receive_image()
        tagger.elaborate_image(image)
        tags = tagger.get_tags()
        print(tags)
        # TODO: sistemare l'invio dei tag al server
        # TODO: aggiungere il db
        connection.send_response(tags)
        os.remove(image)


def main():
    config = configparser.ConfigParser()
    config.read("config/config.ini")
    HOST = config.get("Tagger", "Hostname")
    PORT = int(config.get("Tagger", "Port"))
    N_TAGS = int(config.get("Tagger", "Tags"))

    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        print("Listen incoming connections")
        s.bind((HOST, PORT))
        s.listen()
        while True:
            conn, addr = s.accept()
            print("Start thread")
            threading.Thread(target=tag_image, args=(
                conn, addr, N_TAGS)).start()


if __name__ == "__main__":
    main()
