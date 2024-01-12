import tkinter as tk
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
        self.connect_button = tk.Button(first_frame, text="Collega al DB", command=self.connect_to_mongo)
        self.connect_button.pack(side=tk.LEFT)


        self.get_data_button = tk.Button(second_frame, text="Ottieni dati", command=self.get_data)
        self.get_data_button.pack(side=tk.LEFT)

        self.textfield = tk.Text(second_frame, height=2, width=5)
        self.textfield.insert("1.0","0")
        self.textfield.pack(side=tk.RIGHT)

        self.plot_button = tk.Button(third_frame, text="Plot", command=self.plot)
        self.plot_button.pack(side=tk.LEFT)

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
        # Leggo il file di configurazione per sapere a quale database collegarmi, quali siano le credenziali di accesso
        # e la collezione da usare
        config = configparser.ConfigParser()
        config.read("config/config.ini")

        HOSTNAME = config.get('MongoDB', 'Hostname')
        PORT = config.get('MongoDB', 'Port')
        USERNAME = config.get('MongoDB', 'Username')
        PASSWORD = config.get('MongoDB', 'Password')
        DATABASE = config.get('MongoDB', 'Database')
        COLLECTION = config.get('MongoDB', 'Collection')

        # Mi collego al db e se tutto va bene lo segnalo all'utente tramite una label (scritta in verde)
        # Se il collegamento non va a buon fine la scritta nella label sarà di colore rosso
        try:
            self.mongoDB = MongoDB(HOSTNAME, PORT, USERNAME, PASSWORD, DATABASE, COLLECTION)
            self.connect_label.configure(fg="green", text="Connessione riuscita")
        except errors.ServerSelectionTimeoutError as c:
            # Notifico l'utente che il database non è disponibile
            self.connect_label.configure(fg="red", text="Server Mongo non disponibile")

    def get_data(self):
        # In questa funzione ottengo i tag dal db e notifico l'utente sul corretto
        # ottenimento tramite la relativa label
        threshold = self.textfield.get("1.0", "end")
        # Verifico se il valore inserito sia numerico; nel caso non lo fosse lo notifico all'utente
        if threshold.strip().isdigit():
            try:
                # Ottengo i dati dal db
                self.tags = self.mongoDB.getTags(int(threshold))
                self.get_label.configure(fg="green", text="Dati ottenuti")
            except AttributeError as ae:
                # In caso di errore nell'ottenimento dei tag
                # notifico l'utente attraverso questa label
                self.get_label.configure(fg="red", text="Dati non ottenuti")
        else:
            self.get_label.configure(fg="red", text="Inserisci un valore numerico")

    def plot(self):
        # In questa funzione plotto, come bar-chart, i dati ottenuti
        try:
            keys = list(self.tags.keys())
            values = list(self.tags.values())

            figure = Figure(figsize=(5, 5), dpi=100)

            # creo la canvas su cui verrà plottato il grafico
            self.figure_canvas = FigureCanvasTkAgg(figure, self)

            # creato la toolbar da associare al grafico
            self.toolbar = NavigationToolbar2Tk(self.figure_canvas, self)

            # creo gli assi
            axes = figure.add_subplot()
            axes.grid(visible=True, color="grey", alpha=0.2)

            # creo gli assi x-y assegnado loro i valori: y = keys, x = values
            axes.barh(keys, values)
            # imposto un nome per il grafico
            axes.set_title('Tags')
            # imposto un nome per l'asse delle ascisse
            axes.set_xlabel('Occurance')

            # Inserisco il grafico nell'interfaccia
            self.figure_canvas.get_tk_widget().pack(side=tk.TOP, fill=tk.BOTH, expand=1)
            self.plot_label.configure(text="", fg="green")
        except AttributeError as ae:
            # Notifico l'utente che non vi sono ancora dati da elaborare
            self.plot_label.configure(text="Dati non presenti", fg="red")



if __name__ == "__main__":
    app = Analyzer()
    app.mainloop()
