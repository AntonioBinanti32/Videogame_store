package main

import (
	"bufio"
	"encoding/json"
	"fmt"
	"log"
	"net"
	"os"
	"strings"
)

type Response struct {
	Message string `json:"message"`
	Token   string `json:"token"`
}

func main() {
	conn, err := net.Dial("tcp", "localhost:1984")
	if err != nil {
		log.Fatalf("Failed to connect to server: %v", err)
	}
	defer conn.Close()

	fmt.Printf("Connected to server: %+v\n", conn)

	// Lettura del messaggio dall'utente
	reader := bufio.NewReader(os.Stdin)
	fmt.Print("Enter message to send: ")
	message, err := reader.ReadString('\n')
	if err != nil {
		log.Fatalf("Failed to read message: %v", err)
	}

	// Invio del messaggio al server
	fmt.Fprintf(conn, message)
	fmt.Printf("Message sent: %s", message)

	// Lettura della risposta dal server
	response, err := bufio.NewReader(conn).ReadString('|')
	if err != nil {
		log.Fatalf("Failed to read response from server: %v", err)
	}

	// Elimino il carattere '|'
	response = strings.TrimSuffix(response, "|")

	// Analisi della risposta JSON
	var jsonResponse Response
	err = json.Unmarshal([]byte(response), &jsonResponse)
	if err != nil {
		log.Fatalf("Failed to parse JSON response: %v", err)
	}

	// Stampare il messaggio e il token
	fmt.Printf("Received from server:\nMessage: %s,\n Token: %s\n", jsonResponse.Message, jsonResponse.Token)
}
