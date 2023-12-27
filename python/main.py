import socket
import configparser
import threading
import os
from pymongo import errors
from tagger.tagger import Tagger
from connection.connection import Connection
from database.mongoDB import MongoDB


def tag_image(conn, addr, n_tags, mongo):
    id = threading.get_ident()
    with conn:
        tagger = Tagger(n_tags, mongo)
        connection = Connection(conn, addr, id)
        image = connection.receive_image()
        tagger.elaborate_image(image)
        tags = tagger.get_tags()
        print(tags)
        connection.send_response(tags)
        os.remove(image)


def main():
    config = configparser.ConfigParser()
    config.read("config/config.ini")
    HOST = config.get("Tagger", "Hostname")
    PORT = int(config.get("Tagger", "Port"))
    N_TAGS = int(config.get("Tagger", "Tags"))

    HOSTNAME_DB = config.get('MongoDB', 'Hostname')
    PORT_DB = config.get('MongoDB', 'Port')
    USERNAME = config.get('MongoDB', 'Username')
    PASSWORD = config.get('MongoDB', 'Password')
    DATABASE = config.get('MongoDB', 'Database')
    COLLECTION = config.get('MongoDB', 'Collection')
    
    try:
        mongo = MongoDB(HOSTNAME_DB, PORT_DB, USERNAME, PASSWORD, DATABASE, COLLECTION)
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
            print("Listen incoming connections")
            s.bind((HOST, PORT))
            s.listen()
            while True:
                conn, addr = s.accept()
                print("Start thread")
                threading.Thread(target=tag_image, args=(
                    conn, addr, N_TAGS, mongo)).start()
    except errors.ServerSelectionTimeoutError as c:
        print("Server Mongo non disponibile")


if __name__ == "__main__":
    main()
