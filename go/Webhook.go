package main

import (
	"bytes"
	"encoding/json"
	"fmt"
	"net/http"
)

// Funzione per inviare una notifica al frontend Flask
func Notify(payload WebhookPayload, webhookPort string) error {
	// Conversione del payload della notifica in JSON
	notificationBytes, err := json.Marshal(payload)
	if err != nil {
		return err
	}

	appURL := "http://localhost:" + webhookPort + "/receive_notification"

	// Invio della richiesta POST all'endpoint Flask
	resp, err := http.Post(appURL, "application/json", bytes.NewBuffer(notificationBytes))
	if err != nil {
		return err
	}
	defer resp.Body.Close()

	// Controllo se la risposta ha uno status code 200 OK
	if resp.StatusCode != http.StatusOK {
		return fmt.Errorf("received non-200 response: %d", resp.StatusCode)
	}

	return nil
}
