package main

import (
	"database/sql"
	"encoding/json"
	"fmt"
	"time"

	_ "github.com/mattn/go-sqlite3"
)

// Funzione per inizializzare il database e le tabelle
func setupDatabase() (*sql.DB, error) {
	db, err := sql.Open("sqlite3", "notifications.db")
	if err != nil {
		return nil, err
	}

	// Creazione della tabella delle notifiche se non esiste già
	_, err = db.Exec(`CREATE TABLE IF NOT EXISTS notifications (
		id INTEGER PRIMARY KEY AUTOINCREMENT,
		message TEXT,
		timestamp DATETIME
	);`)
	if err != nil {
		return nil, err
	}

	// Creazione della tabella dello stato di lettura se non esiste già
	_, err = db.Exec(`CREATE TABLE IF NOT EXISTS notification_reads (
		notification_id INTEGER,
		username TEXT,
		read INTEGER,
		PRIMARY KEY(notification_id, username),
		FOREIGN KEY(notification_id) REFERENCES notifications(id)
	);`)
	if err != nil {
		return nil, err
	}

	return db, nil
}

// Funzione per aggiungere una nuova notifica al database
func addNotification(db *sql.DB, message string) (int64, error) {
	tx, err := db.Begin()
	if err != nil {
		return 0, err
	}
	defer tx.Rollback()

	// Inserimento della nuova notifica
	result, err := tx.Exec("INSERT INTO notifications (message, timestamp) VALUES (?, ?);", message, time.Now())
	if err != nil {
		return 0, err
	}

	notificationID, err := result.LastInsertId()
	if err != nil {
		return 0, err
	}

	users, err := GetAllUsers(socketTCPPort)

	if err != nil {
		return 0, fmt.Errorf("failed to parse JSON: %v", err)
	}

	for _, user := range users {
		_, err := tx.Exec("INSERT INTO notification_reads (notification_id, username, read) VALUES (?, ?, ?);", notificationID, user.Username, false)
		if err != nil {
			return 0, err
		}
	}

	err = tx.Commit()
	if err != nil {
		return 0, err
	}

	return notificationID, nil
}

// Funzione per aggiungere una nuova notifica al database
func addNotificationToAdmin(db *sql.DB, message string) (int64, error) {
	tx, err := db.Begin()
	if err != nil {
		return 0, err
	}
	defer tx.Rollback()

	// Inserimento della nuova notifica
	result, err := tx.Exec("INSERT INTO notifications (message, timestamp) VALUES (?, ?);", message, time.Now())
	if err != nil {
		return 0, err
	}

	notificationID, err := result.LastInsertId()
	if err != nil {
		return 0, err
	}

	users, err := GetAllUsers(socketTCPPort)

	if err != nil {
		return 0, fmt.Errorf("failed to parse JSON: %v", err)
	}

	for _, user := range users {
		if user.Username == "admin" {
			_, err := tx.Exec("INSERT INTO notification_reads (notification_id, username, read) VALUES (?, ?, ?);", notificationID, "admin", false)
			if err != nil {
				return 0, err
			}
		}

	}

	err = tx.Commit()
	if err != nil {
		return 0, err
	}

	return notificationID, nil
}

// Funzione per segnare una notifica come letta per un utente specifico
func markNotificationAsRead(db *sql.DB, notificationID int64, username string) error {
	_, err := db.Exec("UPDATE notification_reads SET read = 1 WHERE notification_id = ? AND username = ?;", notificationID, username)
	if err != nil {
		return err
	}
	return nil
}

// Funzione per ottenere tutte le notifiche non lette di un utente
func getUnreadNotifications(db *sql.DB, username string) ([]Notification, error) {
	rows, err := db.Query(`
		SELECT n.id, n.message, n.timestamp
		FROM notifications n
		LEFT JOIN notification_reads nr ON n.id = nr.notification_id AND nr.username = ?
		WHERE nr.read IS NULL OR nr.read = 0;
	`, username)
	if err != nil {
		return nil, err
	}
	defer rows.Close()

	var notifications []Notification

	for rows.Next() {
		var notification Notification
		err := rows.Scan(&notification.ID, &notification.Message, &notification.Timestamp)
		if err != nil {
			return nil, err
		}
		notifications = append(notifications, notification)
	}

	return notifications, nil
}

// Funzione per ottenere tutte le notifiche di un utente
func getAllNotifications(db *sql.DB, userID string) ([]Notification, error) {
	rows, err := db.Query(`
		SELECT n.id, n.message, n.timestamp
		FROM notifications n
		LEFT JOIN notification_reads nr ON n.id = nr.notification_id AND nr.username = ?;
	`, userID)
	if err != nil {
		return nil, err
	}
	defer rows.Close()

	var notifications []Notification

	for rows.Next() {
		var notification Notification
		err := rows.Scan(&notification.ID, &notification.Message, &notification.Timestamp)
		if err != nil {
			return nil, err
		}
		notifications = append(notifications, notification)
	}

	return notifications, nil
}

// Funzione per ottenere tutti gli utenti
func GetAllUsers(socketTCPPort string) ([]User, error) {

	message := "getAllUsers***"

	jsonResponse, err := communicateWithBackend(message, socketTCPPort)
	if err != nil {
		return nil, fmt.Errorf("failed to communicate with backend: %v", err)
	}
	if jsonResponse.Error {
		return nil, fmt.Errorf("backend error: %v", jsonResponse.Message)
	}

	var users []User
	err = json.Unmarshal([]byte(jsonResponse.Message), &users)
	if err != nil {
		return nil, fmt.Errorf("failed to parse user list: %v", err)
	}

	return users, nil
}

// Funzione per cancellare tutto il contenuto del database
func clearDatabase(db *sql.DB) error {
	_, err := db.Exec("DELETE FROM notification_reads;")
	if err != nil {
		return err
	}
	_, err = db.Exec("DELETE FROM notifications;")
	if err != nil {
		return err
	}
	return nil
}
