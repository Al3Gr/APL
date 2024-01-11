import socket
import configparser
import threading
import os
from pymongo import errors
from tagger.tagger import Tagger
from connection.connection import Connection
from database.mongoDB import MongoDB


def tag_image(conn, addr, n_tags, threshold, mongo):
    # Funzione che si occupa della ricezione dell'immagine,
    # della sua inferenza e della trasmissione dei tag ottenuti al server
    id = threading.get_ident()
    with conn:
        tagger = Tagger(n_tags, threshold)
        connection = Connection(conn, addr, id)
        # ricevo l'immagine, la elaboro ed ottengo i relativi tag
        image = connection.receive_image()
        tagger.elaborate_image(image)
        tags = tagger.get_tags()
        # registro i tag ottenuti nel db
        mongo.addTags(tags)
        print(tags)
        # mando i tag al server
        connection.send_response(list(tags.keys()))
        # Cancello l'immagine per evitare di occupare memoria inutilmente dopo la sua inferenza
        # e la restituizione dei relativi tag
        os.remove(image)


def main():
    # Leggo il file di configurazione
    # Setto i parametri per quanto riguarda il tagger:
    # Host e porta in cui si mette in ascolto, numero di tag restituiti
    config = configparser.ConfigParser()
    config.read("config/config.ini")
    HOST = config.get("Tagger", "Hostname")
    PORT = int(config.get("Tagger", "Port"))
    N_TAGS = int(config.get("Tagger", "Tags"))
    THRESHOLD = int(config.get("Tagger", "Threshold"))

    # Setto i parametri per la connessione col il databse mongoDB
    HOSTNAME_DB = config.get('MongoDB', 'Hostname')
    PORT_DB = config.get('MongoDB', 'Port')
    USERNAME = config.get('MongoDB', 'Username')
    PASSWORD = config.get('MongoDB', 'Password')
    DATABASE = config.get('MongoDB', 'Database')
    COLLECTION = config.get('MongoDB', 'Collection')
    
    try:
        # Instaurata la connessione col db, mi pongo in attesa di nuove connessioni in entrata
        mongo = MongoDB(HOSTNAME_DB, PORT_DB, USERNAME, PASSWORD, DATABASE, COLLECTION)
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
            print("Listen incoming connections")
            s.bind((HOST, PORT))
            s.listen()
            # Ogni volta che arriva un anuova connessione, faccio partire un thread che gestirà
            # l'inferenza dell'immagine in arrivo
            while True:
                conn, addr = s.accept()
                print("Start thread")
                threading.Thread(target=tag_image, args=(
                    conn, addr, N_TAGS, THRESHOLD, mongo)).start()
    except errors.ServerSelectionTimeoutError as c:
        print("Server Mongo non disponibile")


if __name__ == "__main__":
    main()
