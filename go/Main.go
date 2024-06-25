package main

import (
	"fmt"
	"log"
	"net/http"

	"github.com/gorilla/mux"
	"github.com/spf13/viper"
)

var socketTCPPort string

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
	socketTCPPort = viper.GetString("SocketTCP.Port")
	webhookPort := viper.GetString("Webhook.Port")

	fmt.Println("Porta del server:", serverPort, "\nPorta del socket:", socketTCPPort)

	// Configurazione del database
	db, err := setupDatabase()
	if err != nil {
		log.Fatal("Errore durante la configurazione del database:", err)
	}
	defer db.Close()

	r := mux.NewRouter()

	// Route per la creazione di un nuovo elemento
	r.HandleFunc("/", HomeHandler).Methods("GET")
	r.HandleFunc("/signup", func(w http.ResponseWriter, r *http.Request) {
		SignupHandler(w, r, socketTCPPort)
	}).Methods("POST")
	r.HandleFunc("/login", func(w http.ResponseWriter, r *http.Request) {
		LoginHandler(w, r, socketTCPPort)
	}).Methods("POST")
	r.HandleFunc("/getUser/{username}", func(w http.ResponseWriter, r *http.Request) {
		params := mux.Vars(r)
		username := params["username"]
		GetUserHandler(w, r, socketTCPPort, username)
	}).Methods("GET")
	r.HandleFunc("/getAllUsers", func(w http.ResponseWriter, r *http.Request) {
		GetAllUsersHandler(w, r, socketTCPPort)
	}).Methods("GET")
	r.HandleFunc("/addGame", func(w http.ResponseWriter, r *http.Request) {
		AddGameHandler(w, r, socketTCPPort, webhookPort, db)
	}).Methods("POST")
	r.HandleFunc("/getGames", func(w http.ResponseWriter, r *http.Request) {
		GetGamesHandler(w, r, socketTCPPort)
	}).Methods("GET")
	r.HandleFunc("/getGameByTitle/{gameTitle}", func(w http.ResponseWriter, r *http.Request) {
		params := mux.Vars(r)
		gameTitle := params["gameTitle"]
		GetGameByTitleHandler(w, r, socketTCPPort, gameTitle)
	}).Methods("GET")
	r.HandleFunc("/getGameByGenre/{genre}", func(w http.ResponseWriter, r *http.Request) {
		params := mux.Vars(r)
		genre := params["genre"]
		GetGameByGenreHandler(w, r, socketTCPPort, genre)
	}).Methods("GET")
	r.HandleFunc("/getUserPreferredGames", func(w http.ResponseWriter, r *http.Request) {
		GetUserPreferredGamesHandler(w, r, socketTCPPort)
	}).Methods("GET")
	r.HandleFunc("/getReview/{reviewID}", func(w http.ResponseWriter, r *http.Request) {
		params := mux.Vars(r)
		reviewID := params["reviewID"]
		GetReviewHandler(w, r, socketTCPPort, reviewID)
	}).Methods("GET")
	r.HandleFunc("/getReviewByUser/{username}", func(w http.ResponseWriter, r *http.Request) {
		params := mux.Vars(r)
		username := params["username"]
		GetReviewByUserHandler(w, r, socketTCPPort, username)
	}).Methods("GET")
	r.HandleFunc("/getReviewByGame/{gameTitle}", func(w http.ResponseWriter, r *http.Request) {
		params := mux.Vars(r)
		gameTitle := params["gameTitle"]
		GetReviewByGameHandler(w, r, socketTCPPort, gameTitle)
	}).Methods("GET")
	r.HandleFunc("/addReview", func(w http.ResponseWriter, r *http.Request) {
		AddReviewHandler(w, r, socketTCPPort, db)
	}).Methods("POST")
	r.HandleFunc("/addReservation", func(w http.ResponseWriter, r *http.Request) {
		AddReservationHandler(w, r, socketTCPPort)
	}).Methods("POST")
	r.HandleFunc("/addPurchase", func(w http.ResponseWriter, r *http.Request) {
		AddPurchaseHandler(w, r, socketTCPPort, db)
	}).Methods("POST")
	r.HandleFunc("/getReservations/{username}", func(w http.ResponseWriter, r *http.Request) {
		params := mux.Vars(r)
		username := params["username"]
		GetReservationsHandler(w, r, socketTCPPort, username)
	}).Methods("GET")
	r.HandleFunc("/getAllPurchases", func(w http.ResponseWriter, r *http.Request) {
		GetAllPurchasesHandler(w, r, socketTCPPort)
	}).Methods("GET")
	r.HandleFunc("/getPurchases/{username}", func(w http.ResponseWriter, r *http.Request) {
		params := mux.Vars(r)
		username := params["username"]
		GetPurchasesHandler(w, r, socketTCPPort, username)
	}).Methods("GET")
	r.HandleFunc("/updateUser", func(w http.ResponseWriter, r *http.Request) {
		UpdateUserHandler(w, r, socketTCPPort)
	}).Methods("PUT")
	r.HandleFunc("/updateGame", func(w http.ResponseWriter, r *http.Request) {
		UpdateGameHandler(w, r, socketTCPPort, db)
	}).Methods("PUT")
	r.HandleFunc("/updateReview", func(w http.ResponseWriter, r *http.Request) {
		UpdateReviewHandler(w, r, socketTCPPort)
	}).Methods("PUT")
	r.HandleFunc("/updateReservation", func(w http.ResponseWriter, r *http.Request) {
		UpdateReservationHandler(w, r, socketTCPPort)
	}).Methods("PUT")
	r.HandleFunc("/updatePurchase", func(w http.ResponseWriter, r *http.Request) {
		UpdatePurchaseHandler(w, r, socketTCPPort)
	}).Methods("PUT")
	r.HandleFunc("/deleteUser/{username}", func(w http.ResponseWriter, r *http.Request) {
		params := mux.Vars(r)
		username := params["username"]
		DeleteUserHandler(w, r, socketTCPPort, username)
	}).Methods("DELETE")
	r.HandleFunc("/deleteGame/{gameTitle}", func(w http.ResponseWriter, r *http.Request) {
		params := mux.Vars(r)
		gameTitle := params["gameTitle"]
		DeleteGameHandler(w, r, socketTCPPort, gameTitle)
	}).Methods("DELETE")
	r.HandleFunc("/deleteReservation/{reservationId}", func(w http.ResponseWriter, r *http.Request) {
		params := mux.Vars(r)
		reservationId := params["reservationId"]
		DeleteReservationHandler(w, r, socketTCPPort, reservationId)
	}).Methods("DELETE")
	r.HandleFunc("/deletePurchase/{purchaseId}", func(w http.ResponseWriter, r *http.Request) {
		params := mux.Vars(r)
		purchaseId := params["purchaseId"]
		DeletePurchaseHandler(w, r, socketTCPPort, purchaseId)
	}).Methods("DELETE")
	r.HandleFunc("/deleteReview", func(w http.ResponseWriter, r *http.Request) {
		DeleteReviewHandler(w, r, socketTCPPort)
	}).Methods("DELETE")
	r.HandleFunc("/getAllNotifications/{username}", func(w http.ResponseWriter, r *http.Request) {
		params := mux.Vars(r)
		username := params["username"]
		getAllNotificationsHandler(w, r, db, username)
	}).Methods("GET")
	r.HandleFunc("/getUnreadNotifications/{username}", func(w http.ResponseWriter, r *http.Request) {
		params := mux.Vars(r)
		username := params["username"]
		getUnreadNotificationsHandler(w, r, db, username)
	}).Methods("GET")
	r.HandleFunc("/markNotificationAsRead", func(w http.ResponseWriter, r *http.Request) {
		markNotificationAsReadHandler(w, r, db)
	}).Methods("POST")

	http.Handle("/", r)

	log.Fatal(http.ListenAndServe(":"+serverPort, nil))
}
