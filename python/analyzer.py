import tkinter as tk
from matplotlib import pyplot
from matplotlib.figure import Figure 
from matplotlib.backends.backend_tkagg import (FigureCanvasTkAgg, 
NavigationToolbar2Tk) 
from database.mongoDB import MongoDB
import configparser
from pymongo import errors


# La classe Analyzer fornisce un'interfaccia grafica al gestore dell'applicazione
# con la quale egli potrà effettuare alcune analisi sui dati raccolti (i tag) dal tagger
class Analyzer(tk.Tk):
    def __init__(self, screenName: str | None = None, baseName: str | None = None, className: str = "Tk", useTk: bool = True, sync: bool = False, use: str | None = None) -> None:
        # In fase di inizializzazione, viene creata l'interfaccia con la quale il gestore avrà a che fare
        super().__init__(screenName, baseName, className, useTk, sync, use)

        # Qui settiamo il titolo della finestra root e le sue dimensioni
        # Root
        self.title("Analyzer")
        self.geometry("800x800")

        # Suddividiamo la root in 3 sub-frame, ognuna con un proprio padding
        first_frame = tk.Frame(self)
        first_frame.pack(fill='x', padx=20, pady=10)

        second_frame = tk.Frame(self)
        second_frame.pack(fill='x', padx=20, pady=10)

        third_frame = tk.Frame(self)
        third_frame.pack(fill='x', padx=20, pady=10)

        # Creiamo i buttons che andranno inseriti nell'interfaccia
        # Per ognuno di essi, specifichiamo il frame di appartenenza, il titolo e il comando da eseguire
        connect_button = tk.Button(first_frame, text="Collega al DB", command=self.connect_to_mongo)
        connect_button.pack(side=tk.LEFT)


        get_data_button = tk.Button(second_frame, text="Ottieni dati", command=self.get_data)
        get_data_button.pack(side=tk.LEFT)

        plot_button = tk.Button(third_frame, text="Plot", command=self.plot)
        plot_button.pack(side=tk.LEFT)

        # Creiamo anche delle label con la quale restituire
        # delle informazioni sull'esito delle varie operazioni all'utente
        self.connect_label = tk.Label(first_frame)
        self.connect_label.pack(side=tk.LEFT)

        self.get_label = tk.Label(second_frame)
        self.get_label.pack(side=tk.LEFT)

        self.plot_label = tk.Label(third_frame)
        self.plot_label.pack(side=tk.LEFT)


    def connect_to_mongo(self):
        # In questa funzione ci colleghiamo al db dal quale ottenere i dati
        config = configparser.ConfigParser()
        config.read("config/config.ini")

        HOSTNAME = config.get('MongoDB', 'Hostname')
        PORT = config.get('MongoDB', 'Port')
        USERNAME = config.get('MongoDB', 'Username')
        PASSWORD = config.get('MongoDB', 'Password')
        DATABASE = config.get('MongoDB', 'Database')
        COLLECTION = config.get('MongoDB', 'Collection')

        try:
            self.mongoDB = MongoDB(HOSTNAME, PORT, USERNAME, PASSWORD, DATABASE, COLLECTION)
            self.connect_label.configure(fg="green", text="Connessione riuscita")
        except errors.ServerSelectionTimeoutError as c:
            # Notifico l'utente che il database non è disponibile
            self.connect_label.configure(fg="red", text="Server Mongo non disponibile")

    def get_data(self):
        # In questa funzione ottengo i tag dal db e notifico l'utente sul corretto
        # ottenimento tramite la relativa label
        try:
            self.tags = self.mongoDB.getTags()
            self.get_label.configure(fg="green", text="Dati ottenuti")
        except AttributeError as ae:
            # In caso di errore nell'ottenimento dei tag
            # notifico l'utente attraverso questa label
            self.get_label.configure(fg="red", text="Dati non ottenuti")

    def plot(self):
        # In questa funzione plotto, come bar chart, i dati ottenuti
        try:
            keys = list(self.tags.keys())
            values = list(self.tags.values())

            figure = Figure(figsize=(5, 5), dpi=100)

            # create FigureCanvasTkAgg object
            figure_canvas = FigureCanvasTkAgg(figure, self)

            # create the toolbar
            NavigationToolbar2Tk(figure_canvas, self)

            # create axes
            axes = figure.add_subplot()
            axes.grid(visible=True, color="grey", alpha=0.2)

            # create the barchart
            axes.barh(keys, values)
            axes.set_title('Tags')
            axes.set_xlabel('Popularity')

            figure_canvas.get_tk_widget().pack(side=tk.TOP, fill=tk.BOTH, expand=1)
            self.plot_label.configure(text="", fg="green")
        except AttributeError as ae:
            # Notifico l'utente che non vi sono ancora dati da elaborare
            self.plot_label.configure(text="Dati non presenti", fg="red")



if __name__ == "__main__":
    app = Analyzer()
    app.mainloop()
