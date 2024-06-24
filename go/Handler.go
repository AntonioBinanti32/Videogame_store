package main

// TODO: Implementare handler per recommendations

import (
	"bufio"
	"database/sql"
	"encoding/json"
	"fmt"
	"log"
	"net"
	"net/http"
	"strconv"
	"strings"
)

func SendMessageHandler(w http.ResponseWriter, r *http.Request, socketTCPPort string) {
	// Estrazione messaggio
	message := r.URL.Query().Get("message")
	if message == "" {
		http.Error(w, "Missing 'message' parameter", http.StatusBadRequest)
		return
	}

	// Invio del messaggio al backend e relativa risposta
	jsonResponse, err := communicateWithBackend(message, socketTCPPort)
	if err != nil {
		http.Error(w, fmt.Sprintf("Failed to communicate with backend: %v", err), http.StatusInternalServerError)
		return
	}

	// Risposta al client HTTP
	w.Header().Set("Content-Type", "application/json")
	json.NewEncoder(w).Encode(jsonResponse)
}

func HomeHandler(w http.ResponseWriter, r *http.Request) {
	fmt.Fprintf(w, "Benvenuto al Videogame Store!")
}

func SignupHandler(w http.ResponseWriter, r *http.Request, socketTCPPort string) {
	// Implementazione della gestione delle richieste di registrazione
	var req SignupRequest
	err := json.NewDecoder(r.Body).Decode(&req)
	if err != nil {
		http.Error(w, fmt.Sprintf("Failed to decode request body: %v", err), http.StatusBadRequest)
		return
	}

	message := "signup*" + req.Username + "*" + req.Password + "*" + req.ImageUrl + "*"

	jsonResponse, err := communicateWithBackendToken(message, socketTCPPort)
	if err != nil {
		http.Error(w, fmt.Sprintf("Failed to communicate with backend: %v", err), http.StatusInternalServerError)
		return
	}
	if jsonResponse.Error {
		http.Error(w, fmt.Sprintf("Failed to communicate with backend: %v", jsonResponse.Message), http.StatusInternalServerError)
		return
	}

	// Risposta al client HTTP
	w.Header().Set("Content-Type", "application/json")
	json.NewEncoder(w).Encode(jsonResponse)
}

func LoginHandler(w http.ResponseWriter, r *http.Request, socketTCPPort string) {
	// Decodifica il corpo della richiesta JSON in una struttura LoginRequest
	var req LoginRequest
	err := json.NewDecoder(r.Body).Decode(&req)
	if err != nil {
		http.Error(w, fmt.Sprintf("Failed to decode request body: %v", err), http.StatusBadRequest)
		return
	}

	message := "login*" + req.Username + "*" + req.Password + "*"

	jsonResponse, err := communicateWithBackendToken(message, socketTCPPort)
	if err != nil {
		http.Error(w, fmt.Sprintf("Failed to communicate with backend: %v", err), http.StatusInternalServerError)
		return
	}
	if jsonResponse.Error {
		http.Error(w, fmt.Sprintf("Failed to communicate with backend: %v", jsonResponse.Message), http.StatusInternalServerError)
		return
	}
	fmt.Printf("%v", jsonResponse)
	// Risposta al client HTTP
	w.Header().Set("Content-Type", "application/json")
	json.NewEncoder(w).Encode(jsonResponse)
}

func GetUserHandler(w http.ResponseWriter, r *http.Request, socketTCPPort string, username string) {

	actualUser := r.Header.Get("ActualUser")
	token := r.Header.Get("Token")

	message := "getUser*" + username + "*" + actualUser + "*" + token + "*"

	jsonResponse, err := communicateWithBackend(message, socketTCPPort)
	if err != nil {
		http.Error(w, fmt.Sprintf("Failed to communicate with backend: %v", err), http.StatusInternalServerError)
		return
	}
	if jsonResponse.Error {
		http.Error(w, fmt.Sprintf("Failed to communicate with backend: %v", jsonResponse.Message), http.StatusInternalServerError)
		return
	}

	jsonMessage := getJsonList(jsonResponse.Message)
	w.Header().Set("Content-Type", "application/json")
	json.NewEncoder(w).Encode(jsonMessage)
}

func GetAllUsersHandler(w http.ResponseWriter, r *http.Request, socketTCPPort string) {

	message := "getAllUsers***"

	jsonResponse, err := communicateWithBackend(message, socketTCPPort)
	if err != nil {
		http.Error(w, fmt.Sprintf("Failed to communicate with backend: %v", err), http.StatusInternalServerError)
		return
	}
	if jsonResponse.Error {
		http.Error(w, fmt.Sprintf("Failed to communicate with backend: %v", jsonResponse.Message), http.StatusInternalServerError)
		return
	}

	jsonMessage := getJsonList(jsonResponse.Message)
	w.Header().Set("Content-Type", "application/json")
	json.NewEncoder(w).Encode(jsonMessage)
}

func AddGameHandler(w http.ResponseWriter, r *http.Request, socketTCPPort string, webhookPort string, db *sql.DB) {
	var req AddGameRequest
	err := json.NewDecoder(r.Body).Decode(&req)
	if err != nil {
		http.Error(w, fmt.Sprintf("Failed to decode request body: %v", err), http.StatusBadRequest)
		return
	}

	actualUser := r.Header.Get("ActualUser")
	token := r.Header.Get("Token")

	message := "addGame*" + req.Title + "*" + req.Genre + "*" + req.ReleaseDate + "*" + req.Developer + "*" + fmt.Sprintf("%f", req.Price) + "*" + fmt.Sprintf("%d", req.Stock) + "*" + req.Description + "*" + req.ImageUrl + "*" + actualUser + "*" + token + "*"

	jsonResponse, err := communicateWithBackend(message, socketTCPPort)
	if err != nil {
		http.Error(w, fmt.Sprintf("Failed to communicate with backend: %v", err), http.StatusInternalServerError)
		return
	}
	if jsonResponse.Error {
		http.Error(w, fmt.Sprintf("Failed to communicate with backend: %v", jsonResponse.Message), http.StatusInternalServerError)
		return
	}

	notify := fmt.Sprintf("'%s' is now available in our store: take a look!", req.Title)

	addNotification(db, notify)

	w.Header().Set("Content-Type", "application/json")
	json.NewEncoder(w).Encode(jsonResponse)
}

func GetGamesHandler(w http.ResponseWriter, r *http.Request, socketTCPPort string) {

	actualUser := r.Header.Get("ActualUser")
	token := r.Header.Get("Token")

	message := "getGames*" + actualUser + "*" + token + "*"

	jsonResponse, err := communicateWithBackend(message, socketTCPPort)
	if err != nil {
		http.Error(w, fmt.Sprintf("Failed to communicate with backend: %v", err), http.StatusInternalServerError)
		return
	}
	if jsonResponse.Error {
		http.Error(w, fmt.Sprintf("Failed to communicate with backend: %v", jsonResponse.Message), http.StatusInternalServerError)
		return
	}

	jsonMessage := getJsonList(jsonResponse.Message)
	w.Header().Set("Content-Type", "application/json")
	json.NewEncoder(w).Encode(jsonMessage)
}

func GetGameByTitleHandler(w http.ResponseWriter, r *http.Request, socketTCPPort string, gameTitle string) {

	actualUser := r.Header.Get("ActualUser")
	token := r.Header.Get("Token")

	message := "getGameByTitle*" + gameTitle + "*" + actualUser + "*" + token + "*"

	jsonResponse, err := communicateWithBackend(message, socketTCPPort)
	if err != nil {
		http.Error(w, fmt.Sprintf("Failed to communicate with backend: %v", err), http.StatusInternalServerError)
		return
	}
	if jsonResponse.Error {
		http.Error(w, fmt.Sprintf("Failed to communicate with backend: %v", jsonResponse.Message), http.StatusInternalServerError)
		return
	}

	jsonMessage := getJsonList(jsonResponse.Message)
	w.Header().Set("Content-Type", "application/json")
	json.NewEncoder(w).Encode(jsonMessage)
}

func GetGameByGenreHandler(w http.ResponseWriter, r *http.Request, socketTCPPort string, genre string) {

	actualUser := r.Header.Get("ActualUser")
	token := r.Header.Get("Token")

	message := "getGameByGenre*" + genre + "*" + actualUser + "*" + token + "*"

	jsonResponse, err := communicateWithBackend(message, socketTCPPort)
	if err != nil {
		http.Error(w, fmt.Sprintf("Failed to communicate with backend: %v", err), http.StatusInternalServerError)
		return
	}
	if jsonResponse.Error {
		http.Error(w, fmt.Sprintf("Failed to communicate with backend: %v", jsonResponse.Message), http.StatusInternalServerError)
		return
	}

	jsonMessage := getJsonList(jsonResponse.Message)
	w.Header().Set("Content-Type", "application/json")
	json.NewEncoder(w).Encode(jsonMessage)
}

func GetReviewHandler(w http.ResponseWriter, r *http.Request, socketTCPPort string, reviewID string) {

	actualUser := r.Header.Get("ActualUser")
	token := r.Header.Get("Token")

	message := "getReview*" + reviewID + "*" + actualUser + "*" + token + "*"

	jsonResponse, err := communicateWithBackend(message, socketTCPPort)
	if err != nil {
		http.Error(w, fmt.Sprintf("Failed to communicate with backend: %v", err), http.StatusInternalServerError)
		return
	}
	if jsonResponse.Error {
		http.Error(w, fmt.Sprintf("Failed to communicate with backend: %v", jsonResponse.Message), http.StatusInternalServerError)
		return
	}

	jsonMessage := getJsonList(jsonResponse.Message)
	w.Header().Set("Content-Type", "application/json")
	json.NewEncoder(w).Encode(jsonMessage)
}

func GetReviewByUserHandler(w http.ResponseWriter, r *http.Request, socketTCPPort string, username string) {

	actualUser := r.Header.Get("ActualUser")
	token := r.Header.Get("Token")

	message := "getReviewByUser*" + username + "*" + actualUser + "*" + token + "*"

	jsonResponse, err := communicateWithBackend(message, socketTCPPort)
	if err != nil {
		http.Error(w, fmt.Sprintf("Failed to communicate with backend: %v", err), http.StatusInternalServerError)
		return
	}
	if jsonResponse.Error {
		http.Error(w, fmt.Sprintf("Failed to communicate with backend: %v", jsonResponse.Message), http.StatusInternalServerError)
		return
	}

	jsonMessage := getJsonList(jsonResponse.Message)
	w.Header().Set("Content-Type", "application/json")
	json.NewEncoder(w).Encode(jsonMessage)
}

func GetReviewByGameHandler(w http.ResponseWriter, r *http.Request, socketTCPPort string, gameTitle string) {

	actualUser := r.Header.Get("ActualUser")
	token := r.Header.Get("Token")

	message := "getReviewByGame*" + gameTitle + "*" + actualUser + "*" + token + "*"

	jsonResponse, err := communicateWithBackend(message, socketTCPPort)
	if err != nil {
		http.Error(w, fmt.Sprintf("Failed to communicate with backend: %v", err), http.StatusInternalServerError)
		return
	}
	if jsonResponse.Error {
		http.Error(w, fmt.Sprintf("Failed to communicate with backend: %v", jsonResponse.Message), http.StatusInternalServerError)
		return
	}

	jsonMessage := getJsonList(jsonResponse.Message)
	w.Header().Set("Content-Type", "application/json")
	json.NewEncoder(w).Encode(jsonMessage)
}

func AddReviewHandler(w http.ResponseWriter, r *http.Request, socketTCPPort string, db *sql.DB) {
	var req AddReviewRequest
	err := json.NewDecoder(r.Body).Decode(&req)
	if err != nil {
		http.Error(w, fmt.Sprintf("Failed to decode request body: %v", err), http.StatusBadRequest)
		return
	}

	actualUser := r.Header.Get("ActualUser")
	token := r.Header.Get("Token")

	message := "addReview*" + req.Username + "*" + req.GameTitle + "*" + req.ReviewText + "*" + strconv.Itoa(req.Rating) + "*" + actualUser + "*" + token + "*"

	jsonResponse, err := communicateWithBackend(message, socketTCPPort)
	if err != nil {
		http.Error(w, fmt.Sprintf("Failed to communicate with backend: %v", err), http.StatusInternalServerError)
		return
	}
	if jsonResponse.Error {
		http.Error(w, fmt.Sprintf("Failed to communicate with backend: %v", jsonResponse.Message), http.StatusInternalServerError)
		return
	}

	notify := fmt.Sprintf("User %s wrote a review for game %s", req.Username, req.GameTitle)
	addNotificationToAdmin(db, notify)

	w.Header().Set("Content-Type", "application/json")
	json.NewEncoder(w).Encode(jsonResponse)
}

func AddReservationHandler(w http.ResponseWriter, r *http.Request, socketTCPPort string) {
	var req AddReservationRequest
	err := json.NewDecoder(r.Body).Decode(&req)
	if err != nil {
		http.Error(w, fmt.Sprintf("Failed to decode request body: %v", err), http.StatusBadRequest)
		return
	}

	actualUser := r.Header.Get("ActualUser")
	token := r.Header.Get("Token")

	message := "addReservation*" + req.Username + "*" + req.GameTitle + "*" + strconv.Itoa(req.NumCopies) + "*" + actualUser + "*" + token + "*"

	jsonResponse, err := communicateWithBackend(message, socketTCPPort)
	if err != nil {
		http.Error(w, fmt.Sprintf("Failed to communicate with backend: %v", err), http.StatusInternalServerError)
		return
	}
	if jsonResponse.Error {
		http.Error(w, fmt.Sprintf("Failed to communicate with backend: %v", jsonResponse.Message), http.StatusInternalServerError)
		return
	}

	w.Header().Set("Content-Type", "application/json")
	json.NewEncoder(w).Encode(jsonResponse)
}

func AddPurchaseHandler(w http.ResponseWriter, r *http.Request, socketTCPPort string, db *sql.DB) {
	var req AddPurchaseRequest
	err := json.NewDecoder(r.Body).Decode(&req)
	if err != nil {
		http.Error(w, fmt.Sprintf("Failed to decode request body: %v", err), http.StatusBadRequest)
		return
	}

	actualUser := r.Header.Get("ActualUser")
	token := r.Header.Get("Token")

	message := "addPurchase*" + req.Username + "*" + req.GameTitle + "*" + strconv.Itoa(req.NumCopies) + "*" + actualUser + "*" + token + "*"

	jsonResponse, err := communicateWithBackend(message, socketTCPPort)
	if err != nil {
		http.Error(w, fmt.Sprintf("Failed to communicate with backend: %v", err), http.StatusInternalServerError)
		return
	}
	if jsonResponse.Error {
		http.Error(w, fmt.Sprintf("Failed to communicate with backend: %v", jsonResponse.Message), http.StatusInternalServerError)
		return
	}

	notify := fmt.Sprintf("%d copies of game '%s' were purchased by user %s", req.NumCopies, req.GameTitle, actualUser)
	addNotificationToAdmin(db, notify)

	message_2 := "getNumCopiesByGameTitle*" + req.GameTitle + "*" + actualUser + "*" + token + "*"
	gameResponse, err := communicateWithBackend(message_2, socketTCPPort)
	if err != nil {
		http.Error(w, fmt.Sprintf("Failed to fetch game details after purchase: %v", err), http.StatusInternalServerError)
		return
	}

	if gameResponse.Error {
		http.Error(w, fmt.Sprintf("Failed to fetch game details after purchase: %v", gameResponse.Message), http.StatusInternalServerError)
		return
	}

	stock, err := strconv.Atoi(gameResponse.Message)

	if err != nil {
		http.Error(w, fmt.Sprintf("Failed to fetch game details after purchase: %v", err), http.StatusInternalServerError)
		return
	}

	if stock <= 1 {
		notify := fmt.Sprintf("The Game '%s' has only %d copies left!", req.GameTitle, stock)
		addNotificationToAdmin(db, notify)
	}

	w.Header().Set("Content-Type", "application/json")
	json.NewEncoder(w).Encode(jsonResponse)
}

func GetReservationsHandler(w http.ResponseWriter, r *http.Request, socketTCPPort string, username string) {

	actualUser := r.Header.Get("ActualUser")
	token := r.Header.Get("Token")

	message := "getReservations*" + username + "*" + actualUser + "*" + token + "*"

	jsonResponse, err := communicateWithBackend(message, socketTCPPort)
	if err != nil {
		http.Error(w, fmt.Sprintf("Failed to communicate with backend: %v", err), http.StatusInternalServerError)
		return
	}
	if jsonResponse.Error {
		http.Error(w, fmt.Sprintf("Failed to communicate with backend: %v", jsonResponse.Message), http.StatusInternalServerError)
		return
	}
	jsonMessage := getJsonList(jsonResponse.Message)
	w.Header().Set("Content-Type", "application/json")
	json.NewEncoder(w).Encode(jsonMessage)
}

func GetAllPurchasesHandler(w http.ResponseWriter, r *http.Request, socketTCPPort string) {

	actualUser := r.Header.Get("ActualUser")
	token := r.Header.Get("Token")

	message := "getAllPurchases*" + actualUser + "*" + token + "*"

	jsonResponse, err := communicateWithBackend(message, socketTCPPort)
	if err != nil {
		http.Error(w, fmt.Sprintf("Failed to communicate with backend: %v", err), http.StatusInternalServerError)
		return
	}
	if jsonResponse.Error {
		http.Error(w, fmt.Sprintf("Failed to communicate with backend: %v", jsonResponse.Message), http.StatusInternalServerError)
		return
	}

	jsonMessage := getJsonList(jsonResponse.Message)
	w.Header().Set("Content-Type", "application/json")
	json.NewEncoder(w).Encode(jsonMessage)
}

func GetPurchasesHandler(w http.ResponseWriter, r *http.Request, socketTCPPort string, username string) {

	actualUser := r.Header.Get("ActualUser")
	token := r.Header.Get("Token")

	message := "getPurchases*" + username + "*" + actualUser + "*" + token + "*"

	jsonResponse, err := communicateWithBackend(message, socketTCPPort)
	if err != nil {
		http.Error(w, fmt.Sprintf("Failed to communicate with backend: %v", err), http.StatusInternalServerError)
		return
	}
	if jsonResponse.Error {
		http.Error(w, fmt.Sprintf("Failed to communicate with backend: %v", jsonResponse.Message), http.StatusInternalServerError)
		return
	}

	jsonMessage := getJsonList(jsonResponse.Message)
	w.Header().Set("Content-Type", "application/json")
	json.NewEncoder(w).Encode(jsonMessage)
}

func UpdateUserHandler(w http.ResponseWriter, r *http.Request, socketTCPPort string) {
	var req UpdateUserRequest
	err := json.NewDecoder(r.Body).Decode(&req)
	if err != nil {
		http.Error(w, fmt.Sprintf("Failed to decode request body: %v", err), http.StatusBadRequest)
		return
	}

	actualUser := r.Header.Get("ActualUser")
	token := r.Header.Get("Token")

	message := "updateUser*" + req.Username + "*" + req.NewPassword + "*" + req.NewImageUrl + "*" + actualUser + "*" + token + "*"

	jsonResponse, err := communicateWithBackend(message, socketTCPPort)
	if err != nil {
		http.Error(w, fmt.Sprintf("Failed to communicate with backend: %v", err), http.StatusInternalServerError)
		return
	}
	if jsonResponse.Error {
		http.Error(w, fmt.Sprintf("Failed to communicate with backend: %v", jsonResponse.Message), http.StatusInternalServerError)
		return
	}

	w.Header().Set("Content-Type", "application/json")
	json.NewEncoder(w).Encode(jsonResponse)
}

func UpdateGameHandler(w http.ResponseWriter, r *http.Request, socketTCPPort string, db *sql.DB) {
	var req UpdateGameRequest
	err := json.NewDecoder(r.Body).Decode(&req)
	if err != nil {
		http.Error(w, fmt.Sprintf("Failed to decode request body: %v", err), http.StatusBadRequest)
		return
	}

	actualUser := r.Header.Get("ActualUser")
	token := r.Header.Get("Token")

	message := "updateGame*" + req.Title + "*" + req.NewGenre + "*" + req.NewReleaseDate + "*" + req.NewDeveloper + "*" + fmt.Sprintf("%f", req.NewPrice) + "*" + strconv.Itoa(req.NewStock) + "*" + req.NewDescription + "*" + req.NewImageUrl + "*" + actualUser + "*" + token + "*"

	jsonResponse, err := communicateWithBackend(message, socketTCPPort)
	if err != nil {
		http.Error(w, fmt.Sprintf("Failed to communicate with backend: %v", err), http.StatusInternalServerError)
		return
	}
	if jsonResponse.Error {
		http.Error(w, fmt.Sprintf("Failed to communicate with backend: %v", jsonResponse.Message), http.StatusInternalServerError)
		return
	}

	notify := fmt.Sprintf("The Game '%s' is being modified, see what's new!", req.Title)
	addNotification(db, notify)

	w.Header().Set("Content-Type", "application/json")
	json.NewEncoder(w).Encode(jsonResponse)
}

func UpdateReviewHandler(w http.ResponseWriter, r *http.Request, socketTCPPort string) {
	var req UpdateReviewRequest
	err := json.NewDecoder(r.Body).Decode(&req)
	if err != nil {
		http.Error(w, fmt.Sprintf("Failed to decode request body: %v", err), http.StatusBadRequest)
		return
	}

	actualUser := r.Header.Get("ActualUser")
	token := r.Header.Get("Token")

	message := "updateReview*" + req.Username + "*" + req.GameTitle + "*" + req.NewReviewText + "*" + strconv.Itoa(req.NewRating) + "*" + actualUser + "*" + token + "*"

	jsonResponse, err := communicateWithBackend(message, socketTCPPort)
	if err != nil {
		http.Error(w, fmt.Sprintf("Failed to communicate with backend: %v", err), http.StatusInternalServerError)
		return
	}
	if jsonResponse.Error {
		http.Error(w, fmt.Sprintf("Failed to communicate with backend: %v", jsonResponse.Message), http.StatusInternalServerError)
		return
	}

	w.Header().Set("Content-Type", "application/json")
	json.NewEncoder(w).Encode(jsonResponse)
}

func UpdateReservationHandler(w http.ResponseWriter, r *http.Request, socketTCPPort string) {
	var req UpdateReservationRequest
	err := json.NewDecoder(r.Body).Decode(&req)
	if err != nil {
		http.Error(w, fmt.Sprintf("Failed to decode request body: %v", err), http.StatusBadRequest)
		return
	}

	actualUser := r.Header.Get("ActualUser")
	token := r.Header.Get("Token")

	message := "updateReservation*" + req.Username + "*" + req.GameTitle + "*" + strconv.Itoa(req.NewNumCopies) + "*" + actualUser + "*" + token + "*"

	jsonResponse, err := communicateWithBackend(message, socketTCPPort)
	if err != nil {
		http.Error(w, fmt.Sprintf("Failed to communicate with backend: %v", err), http.StatusInternalServerError)
		return
	}
	if jsonResponse.Error {
		http.Error(w, fmt.Sprintf("Failed to communicate with backend: %v", jsonResponse.Message), http.StatusInternalServerError)
		return
	}

	w.Header().Set("Content-Type", "application/json")
	json.NewEncoder(w).Encode(jsonResponse)
}

func UpdatePurchaseHandler(w http.ResponseWriter, r *http.Request, socketTCPPort string) {
	var req UpdatePurchaseRequest
	err := json.NewDecoder(r.Body).Decode(&req)
	if err != nil {
		http.Error(w, fmt.Sprintf("Failed to decode request body: %v", err), http.StatusBadRequest)
		return
	}

	actualUser := r.Header.Get("ActualUser")
	token := r.Header.Get("Token")

	message := "updatePurchase*" + strconv.Itoa(req.NewNumCopies) + "*" + req.PurchaseID + "*" + actualUser + "*" + token + "*"

	jsonResponse, err := communicateWithBackend(message, socketTCPPort)
	if err != nil {
		http.Error(w, fmt.Sprintf("Failed to communicate with backend: %v", err), http.StatusInternalServerError)
		return
	}
	if jsonResponse.Error {
		http.Error(w, fmt.Sprintf("Failed to communicate with backend: %v", jsonResponse.Message), http.StatusInternalServerError)
		return
	}

	w.Header().Set("Content-Type", "application/json")
	json.NewEncoder(w).Encode(jsonResponse)
}

func DeleteUserHandler(w http.ResponseWriter, r *http.Request, socketTCPPort string, username string) {

	actualUser := r.Header.Get("ActualUser")
	token := r.Header.Get("Token")

	message := "deleteUser*" + username + "*" + actualUser + "*" + token + "*"

	jsonResponse, err := communicateWithBackend(message, socketTCPPort)
	if err != nil {
		http.Error(w, fmt.Sprintf("Failed to communicate with backend: %v", err), http.StatusInternalServerError)
		return
	}
	if jsonResponse.Error {
		http.Error(w, fmt.Sprintf("Failed to communicate with backend: %v", jsonResponse.Message), http.StatusInternalServerError)
		return
	}

	w.Header().Set("Content-Type", "application/json")
	json.NewEncoder(w).Encode(jsonResponse)
}

func DeleteGameHandler(w http.ResponseWriter, r *http.Request, socketTCPPort string, gameTitle string) {

	actualUser := r.Header.Get("ActualUser")
	token := r.Header.Get("Token")

	message := "deleteGame*" + gameTitle + "*" + actualUser + "*" + token + "*"

	jsonResponse, err := communicateWithBackend(message, socketTCPPort)
	if err != nil {
		http.Error(w, fmt.Sprintf("Failed to communicate with backend: %v", err), http.StatusInternalServerError)
		return
	}
	if jsonResponse.Error {
		http.Error(w, fmt.Sprintf("Failed to communicate with backend: %v", jsonResponse.Message), http.StatusInternalServerError)
		return
	}

	w.Header().Set("Content-Type", "application/json")
	json.NewEncoder(w).Encode(jsonResponse)
}

func DeleteReservationHandler(w http.ResponseWriter, r *http.Request, socketTCPPort string, reservationId string) {

	actualUser := r.Header.Get("ActualUser")
	token := r.Header.Get("Token")

	message := "deleteReservation*" + reservationId + "*" + actualUser + "*" + token + "*"

	jsonResponse, err := communicateWithBackend(message, socketTCPPort)
	if err != nil {
		http.Error(w, fmt.Sprintf("Failed to communicate with backend: %v", err), http.StatusInternalServerError)
		return
	}
	if jsonResponse.Error {
		http.Error(w, fmt.Sprintf("Failed to communicate with backend: %v", jsonResponse.Message), http.StatusInternalServerError)
		return
	}

	w.Header().Set("Content-Type", "application/json")
	json.NewEncoder(w).Encode(jsonResponse)
}

func DeletePurchaseHandler(w http.ResponseWriter, r *http.Request, socketTCPPort string, purchaseId string) {

	actualUser := r.Header.Get("ActualUser")
	token := r.Header.Get("Token")

	message := "deletePurchase*" + purchaseId + "*" + actualUser + "*" + token + "*"

	jsonResponse, err := communicateWithBackend(message, socketTCPPort)
	if err != nil {
		http.Error(w, fmt.Sprintf("Failed to communicate with backend: %v", err), http.StatusInternalServerError)
		return
	}
	if jsonResponse.Error {
		http.Error(w, fmt.Sprintf("Failed to communicate with backend: %v", jsonResponse.Message), http.StatusInternalServerError)
		return
	}

	w.Header().Set("Content-Type", "application/json")
	json.NewEncoder(w).Encode(jsonResponse)
}

func DeleteReviewHandler(w http.ResponseWriter, r *http.Request, socketTCPPort string) {
	var req DeleteReviewRequest
	err := json.NewDecoder(r.Body).Decode(&req)
	if err != nil {
		http.Error(w, fmt.Sprintf("Failed to decode request body: %v", err), http.StatusBadRequest)
		return
	}

	actualUser := r.Header.Get("ActualUser")
	token := r.Header.Get("Token")

	message := "deleteReview*" + req.Username + "*" + req.GameTitle + "*" + actualUser + "*" + token + "*"

	jsonResponse, err := communicateWithBackend(message, socketTCPPort)
	if err != nil {
		http.Error(w, fmt.Sprintf("Failed to communicate with backend: %v", err), http.StatusInternalServerError)
		return
	}
	if jsonResponse.Error {
		http.Error(w, fmt.Sprintf("Failed to communicate with backend: %v", jsonResponse.Message), http.StatusInternalServerError)
		return
	}

	w.Header().Set("Content-Type", "application/json")
	json.NewEncoder(w).Encode(jsonResponse)
}

//TODO: Implementare altri handler

func communicateWithBackendToken(message string, socketTCPPort string) (ResponseToken, error) {
	// Connessione al backend tramite socket TCP
	conn, err := net.Dial("tcp", "localhost:"+socketTCPPort) //("tcp", "localhost:1984")
	if err != nil {
		return ResponseToken{}, fmt.Errorf("failed to connect to backend: %v", err)
	}
	defer conn.Close()

	// Invio messaggio al backend
	fmt.Fprintf(conn, message)
	log.Printf("Message sent to backend: %s", message)

	// Lettura risposta dal backend
	response, err := bufio.NewReader(conn).ReadString('|')
	if err != nil {
		return ResponseToken{}, fmt.Errorf("failed to read response from backend: %v", err)
	}

	// Elimino il carattere '|' dalla risposta
	response = strings.TrimSuffix(response, "|")

	var jsonResponse ResponseToken
	//var jsonResponse interface{} TODO: Provare interface
	err = json.Unmarshal([]byte(response), &jsonResponse)
	if err != nil {
		return ResponseToken{}, fmt.Errorf("failed to parse JSON response from backend: %v", err)
	}

	return jsonResponse, nil
}

func communicateWithBackend(message string, socketTCPPort string) (Response, error) {
	// Connessione al backend tramite socket TCP
	conn, err := net.Dial("tcp", "localhost:"+socketTCPPort) //("tcp", "localhost:1984")
	if err != nil {
		return Response{}, fmt.Errorf("failed to connect to backend: %v", err)
	}
	defer conn.Close()

	// Invio messaggio al backend
	fmt.Fprintf(conn, message)
	log.Printf("Message sent to backend: %s", message)

	// Lettura risposta dal backend
	response, err := bufio.NewReader(conn).ReadString('|')
	if err != nil {
		return Response{}, fmt.Errorf("failed to read response from backend: %v", err)
	}

	// Elimino il carattere '|' dalla risposta
	response = strings.TrimSuffix(response, "|")

	var jsonResponse Response
	//var jsonResponse interface{} TODO: Provare interface
	err = json.Unmarshal([]byte(response), &jsonResponse)
	if err != nil {
		return Response{}, fmt.Errorf("failed to parse JSON response from backend: %v", err)
	}

	log.Printf("Message recived from backend: %v\n\n", jsonResponse)

	return jsonResponse, nil
}

func getJsonList(message string) interface{} {
	var result interface{}
	err := json.Unmarshal([]byte(message), &result)
	if err != nil {
		log.Fatalf("Errore durante la decodifica del JSON: %v", err)
	}
	return result
}

func getAllNotificationsHandler(w http.ResponseWriter, r *http.Request, db *sql.DB, username string) {

	notifications, err := getAllNotifications(db, username)
	if err != nil {
		http.Error(w, fmt.Sprintf("Failed to communicate with backend: %v", err), http.StatusInternalServerError)
		return
	}

	jsonResponse, err := json.Marshal(notifications)
	if err != nil {
		http.Error(w, fmt.Sprintf("Failed to encode notifications: %v", err), http.StatusInternalServerError)
		return
	}

	w.Header().Set("Content-Type", "application/json")
	w.WriteHeader(http.StatusOK)
	w.Write(jsonResponse)
}

func getUnreadNotificationsHandler(w http.ResponseWriter, r *http.Request, db *sql.DB, username string) {

	notifications, err := getUnreadNotifications(db, username)
	if err != nil {
		http.Error(w, fmt.Sprintf("Failed to communicate with backend: %v", err), http.StatusInternalServerError)
		return
	}

	jsonResponse, err := json.Marshal(notifications)
	if err != nil {
		http.Error(w, fmt.Sprintf("Failed to encode notifications: %v", err), http.StatusInternalServerError)
		return
	}

	w.Header().Set("Content-Type", "application/json")
	w.WriteHeader(http.StatusOK)
	w.Write(jsonResponse)
}

func markNotificationAsReadHandler(w http.ResponseWriter, r *http.Request, db *sql.DB) {
	var req markNotificationAsReadRequest
	err := json.NewDecoder(r.Body).Decode(&req)
	if err != nil {
		http.Error(w, fmt.Sprintf("Failed to decode request body: %v", err), http.StatusBadRequest)
		return
	}

	notificationID := req.NotificationID
	username := req.Username

	err = markNotificationAsRead(db, int64(notificationID), username)
	if err != nil {
		http.Error(w, fmt.Sprintf("Failed to communicate with backend: %v", err), http.StatusInternalServerError)
		return
	}

	w.Header().Set("Content-Type", "application/json")
	w.WriteHeader(http.StatusOK)
}
