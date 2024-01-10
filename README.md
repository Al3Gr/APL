# Advanced Programming Languange
## Gravagno - Di Mauro

### Librerie utilizzate

- **C++**
  - Restbed (usata per l'implementazione del server)
  - cpp-jwt (usata per l'implementazione del meccanismo di autenticazione token based)
  - mongo-cxx-driver (usata per la comunicazione col database MongoDB)
  - libbson (usata per costruire le query da inviare al database)
  - aws-sdk-cpp (usata per la comunicazione col bucket S3 Minio)
  - nlohmann-json (usata per l'elaborazione dei JSON)
  - inih (usata per la lettura del file di configurazione .ini)

- **Python**
    - torch - torchvision (usate per l'implementazione della rete neurale AlexNET)
    - pymongo (usata per la comunicazione col database MongoDB)
    - tkinter (usata per la creazione dell'interfaccia grafica del analyzer.py)
    - matplotlib (usata per la rappresentazione grafica dei dati nell'analyzer.py)

- **C#**
  - Microsoft.Maui (più di una libreria, è un framework usato per la realizzazione dell'interfaccia grafica multipiattaforma)
  - Newtonsoft.Json (usate per la gestione dei JSON)


### Installazione

- **C++**  
    Utilizzare il gestore di pacchetti _vcpkg_ (in modalità Classic):  
    basterà digitare il seguente commando per installare una delle librerie prima elencate  
    

    vcpkg install "nome_libreria"

- **Python**  
    Utilizzare il gestore di pacchetti _pip_, digitando il seguente comando:
  

    pip install -r requirements.txt

- **C#**  
    Utilizzare il gestore di pachetti _NuGet_, digitando il seguente comando:


    nuget install "nome_libreria"

### Utilizzo

Per far funzionare l'applicazione, bisogna innanzitutto avviare il database mongoDB e il bucket Minio.  
Dopo occore avviare la rete neurale (main.py),ed attendere che l'applicativo python si metta in ascolto  
di nuove connessioni; fatto ciò, si potrà avviare il server c++. Attendere che anche quest'ultimo si sia  
avviato e, una volta fatto ciò, far partire il client C# per iniziare ad utilizzare l'intero applicativo.  
All'avvio del client, l'utente avrà la possibilità sia di registrarsi che di loggarsi nel sistema; una volta fatto ciò,  
l'utente potrà visualizzare le foto caricate sia da lui che da altri utenti così come, avrà anche la possibilità  
di caricare nuove foto.  
Per quanto riguarda il file analyzer, esso permetterà di visualizzare, in forma grafica, alcune informazioni di sistema.

  