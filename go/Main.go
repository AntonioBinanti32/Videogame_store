package main

import (
	"fmt"
	"log"
	"net/http"

	"github.com/gorilla/mux"
	"github.com/spf13/viper"
)

func main() {

	viper.SetConfigName("config")
	viper.SetConfigType("ini")
	viper.AddConfigPath(".")

	// Lettura file di configurazione
	err := viper.ReadInConfig()
	if err != nil {
		fmt.Println("Errore durante la lettura del file di configurazione:", err)
		return
	}

	serverPort := viper.GetString("Server.Port")
	socketTCPPort := viper.GetString("SocketTCP.Port")

	fmt.Println("Porta del server:", serverPort, "\nPorta del socket:", socketTCPPort)

	r := mux.NewRouter()

	// Route per la creazione di un nuovo elemento
	r.HandleFunc("/", HomeHandler).Methods("GET")
	r.HandleFunc("/signup", func(w http.ResponseWriter, r *http.Request) {
		SignupHandler(w, r, socketTCPPort)
	}).Methods("POST")
	r.HandleFunc("/login", func(w http.ResponseWriter, r *http.Request) {
		LoginHandler(w, r, socketTCPPort)
	}).Methods("POST")
	//TODO Implementare altri endpoint

	http.Handle("/", r)

	log.Fatal(http.ListenAndServe(":"+serverPort, nil))

	/* ESEMPI
	r.HandleFunc("/", HomeHandler)
	r.HandleFunc("/signup", SignupHandler).Methods("POST")
	// Route per l'aggiornamento di un elemento esistente
	r.HandleFunc("/item/{id}", UpdateItemHandler).Methods("PUT")

	// Route per l'eliminazione di un elemento esistente
	r.HandleFunc("/item/{id}", DeleteItemHandler).Methods("DELETE")

	// Route per ottenere tutti gli elementi
	r.HandleFunc("/items", GetItemsHandler).Methods("GET")
	http.Handle("/", r)

	log.Fatal(http.ListenAndServe(":8080", r))
	*/
}
