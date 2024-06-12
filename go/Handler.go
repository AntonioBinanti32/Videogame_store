package main

import (
	"bufio"
	"encoding/json"
	"fmt"
	"log"
	"net"
	"net/http"
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

	message := "signup/" + req.Username + "/" + req.Password + "/" + req.ImageUrl + "/"

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

	message := "login/" + req.Username + "/" + req.Password + "/"

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
	err = json.Unmarshal([]byte(response), &jsonResponse)
	if err != nil {
		return Response{}, fmt.Errorf("failed to parse JSON response from backend: %v", err)
	}

	return jsonResponse, nil
}

//TODO: Implementare altri handler
