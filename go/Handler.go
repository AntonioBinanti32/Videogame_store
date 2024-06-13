package main

// TODO: Implementare token
// TODO: Implementare accesso esclusivo ad admin per alcune operazioni
// TODO: Implementare logica delle notifiche
// TODO: Implementare handler per recommendations

import (
	"bufio"
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

	jsonResponse, err := communicateWithBackend(message, socketTCPPort)
	if err != nil {
		http.Error(w, fmt.Sprintf("Failed to communicate with backend: %v", err), http.StatusInternalServerError)
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

	jsonResponse, err := communicateWithBackend(message, socketTCPPort)
	if err != nil {
		http.Error(w, fmt.Sprintf("Failed to communicate with backend: %v", err), http.StatusInternalServerError)
		return
	}
	fmt.Printf("%v", jsonResponse)
	// Risposta al client HTTP
	w.Header().Set("Content-Type", "application/json")
	json.NewEncoder(w).Encode(jsonResponse)
}

func AddGameHandler(w http.ResponseWriter, r *http.Request, socketTCPPort string) {
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

	//var json interface{}
	//err = json.Unmarshal([]byte(jsonResponse.Message), &json)

	w.Header().Set("Content-Type", "application/json")
	json.NewEncoder(w).Encode(jsonResponse)
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

	w.Header().Set("Content-Type", "application/json")
	json.NewEncoder(w).Encode(jsonResponse)
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

	w.Header().Set("Content-Type", "application/json")
	json.NewEncoder(w).Encode(jsonResponse)
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

	w.Header().Set("Content-Type", "application/json")
	json.NewEncoder(w).Encode(jsonResponse)
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

	w.Header().Set("Content-Type", "application/json")
	json.NewEncoder(w).Encode(jsonResponse)
}

func AddReviewHandler(w http.ResponseWriter, r *http.Request, socketTCPPort string) {
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

	message := "addReservation*" + req.Username + "*" + req.GameID + "*" + strconv.Itoa(req.NumCopies) + "*" + actualUser + "*" + token + "*"

	jsonResponse, err := communicateWithBackend(message, socketTCPPort)
	if err != nil {
		http.Error(w, fmt.Sprintf("Failed to communicate with backend: %v", err), http.StatusInternalServerError)
		return
	}

	w.Header().Set("Content-Type", "application/json")
	json.NewEncoder(w).Encode(jsonResponse)
}

func AddPurchaseHandler(w http.ResponseWriter, r *http.Request, socketTCPPort string) {
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

	w.Header().Set("Content-Type", "application/json")
	json.NewEncoder(w).Encode(jsonResponse)
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

	w.Header().Set("Content-Type", "application/json")
	json.NewEncoder(w).Encode(jsonResponse)
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

	w.Header().Set("Content-Type", "application/json")
	json.NewEncoder(w).Encode(jsonResponse)
}

func UpdateGameHandler(w http.ResponseWriter, r *http.Request, socketTCPPort string) {
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

	message := "updatePurchase*" + req.Username + "*" + req.GameTitle + "*" + strconv.Itoa(req.NewNumCopies) + "*" + req.PurchaseID + "*" + actualUser + "*" + token + "*"

	jsonResponse, err := communicateWithBackend(message, socketTCPPort)
	if err != nil {
		http.Error(w, fmt.Sprintf("Failed to communicate with backend: %v", err), http.StatusInternalServerError)
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

	w.Header().Set("Content-Type", "application/json")
	json.NewEncoder(w).Encode(jsonResponse)
}

func DeleteReservationHandler(w http.ResponseWriter, r *http.Request, socketTCPPort string) {
	var req DeleteReservationRequest
	err := json.NewDecoder(r.Body).Decode(&req)
	if err != nil {
		http.Error(w, fmt.Sprintf("Failed to decode request body: %v", err), http.StatusBadRequest)
		return
	}

	actualUser := r.Header.Get("ActualUser")
	token := r.Header.Get("Token")

	message := "deleteReservation*" + req.Username + "*" + req.GameTitle + "*" + actualUser + "*" + token + "*"

	jsonResponse, err := communicateWithBackend(message, socketTCPPort)
	if err != nil {
		http.Error(w, fmt.Sprintf("Failed to communicate with backend: %v", err), http.StatusInternalServerError)
		return
	}

	w.Header().Set("Content-Type", "application/json")
	json.NewEncoder(w).Encode(jsonResponse)
}

func DeletePurchaseHandler(w http.ResponseWriter, r *http.Request, socketTCPPort string) {
	var req DeletePurchaseRequest
	err := json.NewDecoder(r.Body).Decode(&req)
	if err != nil {
		http.Error(w, fmt.Sprintf("Failed to decode request body: %v", err), http.StatusBadRequest)
		return
	}

	actualUser := r.Header.Get("ActualUser")
	token := r.Header.Get("Token")

	message := "deletePurchase*" + req.Username + "*" + req.GameTitle + "*" + req.PurchaseID + "*" + actualUser + "*" + token + "*"

	jsonResponse, err := communicateWithBackend(message, socketTCPPort)
	if err != nil {
		http.Error(w, fmt.Sprintf("Failed to communicate with backend: %v", err), http.StatusInternalServerError)
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

	w.Header().Set("Content-Type", "application/json")
	json.NewEncoder(w).Encode(jsonResponse)
}

//TODO: Implementare altri handler

func communicateWithBackend(message string, socketTCPPort string) ( /*Response*/ interface{}, error) {
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

	return jsonResponse, nil
}
