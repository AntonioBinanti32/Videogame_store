# Advanced Programming Languages

## Antonio Binanti

### Librerie Utilizzate

#### C++
- **mongo-cxx-driver**: Comunicazione col database MongoDB
- **restbed**: Implementazione del server
- **inih[cpp]**: Lettura del file di configurazione `.ini`
- **nlohmann-json**: Elaborazione dei JSON
- **cpp-jwt**: Meccanismo di autenticazione token-based
- **wsock32 ws2_32**: Comunicazione tramite socket TCP Windows

#### Go
- **github.com/gorilla/mux**: Router HTTP per richieste provenienti dal modulo Python
- **github.com/spf13/viper**: Lettura del file `config.ini`

#### Python
- **Flask**: Micro web framework su cui gira l'applicazione Python
- **configparser**: Lettura del file di configurazione `.ini`
- **json**: Gestione dei JSON scambiati con i moduli
- **requests**: Gestione chiamate metodi HTTP
- **datetime**: Manipolazione di date e orari

### Installazione

#### C++
Per l'installazione delle librerie utilizzare il gestore `vcpkg` attraverso il comando: `vcpkg install {nome_libreria}`
Eseguire quindi il comando: `vcpkg integrate install`
Prelevare l'URL di `CMAKE_TOOLCHAIN_FILE` ed inserirlo nel file `CMakeLists.txt` (si trova un set apposito nella riga 2).

#### Go
Da prompt comandi andare sulla directory del progetto Go e digitare: `go mod init {nome_modulo}`.
Questo creerà un file `go.mod`. Quindi digitare: `go get -u {libreria_da_aggiungere}`

#### Python
Per l'installazione delle librerie utilizzare il gestore `miniconda`. Dal prompt di `miniconda` creare un ambiente attraverso il comando: `conda create --name {nome_environment}`
Digitare: `conda activate {nome_environment}`
Installare le librerie necessarie tramite il comando: `conda install {nome_libreria} -y`
Oppure utilizzare il gestore di pacchetti `pip`, digitando il seguente comando: `pip install -r requirements.txt`

### Utilizzo

#### C++
Per compilare il modulo C++ andare nella directory `/backend` e creare una sottodirectory `/build`. Da terminale spostarsi su questa directory e digitare:
`cmake ..`
`cmake --build .`
A questo punto avviare l'eseguibile `Videogame_store.exe` in `/backend/build/debug/`.

#### Go
Per compilare il modulo Go andare nella directory `/go` e da prompt eseguire il comando: `go build`
Avviare quindi l'eseguibile `videogame_store.exe` nella directory `/go/`.

#### Python

### Accesso e Funzionalità

Una volta in esecuzione i tre moduli sarà possibile effettuare il login o la signup al seguente URL: [http://127.0.0.1:5000](http://127.0.0.1:5000).

Una volta effettuata una di queste operazioni con successo sarà possibile accedere alla home dello store. Da qui l'utente avrà la possibilità di:
- Visualizzare tutti i videogiochi ordinati in base agli acquisti e recensioni passate
- Visualizzare le informazioni aggiuntive di ogni videogioco
- Aggiungere videogiochi al carrello e recensirli
- Acquistare tutti i videogiochi aggiunti al carrello

Cliccando sull'icona dell'utente è possibile:
- Visualizzare tutti gli acquisti effettuati dall'utente
- Visualizzare le notifiche
- Effettuare il logout

Tramite l'username "admin" è possibile accedere in modalità amministratore. In questa modalità sarà possibile:
- Aggiungere, modificare o eliminare un videogioco
- Visualizzare tutti gli acquisti fatti dagli utenti
- Visualizzare le attività degli utenti tramite notifiche

Il sistema di notifiche permette di avvisare l'utente ogni volta che viene inserito o modificato un nuovo videogioco nello store, mentre avviserà l'admin ogni volta che un utente effettuerà un'azione su un videogioco.

