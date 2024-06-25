package main

import "time"

type ResponseToken struct {
	Message interface{} `json:"message"`
	Token   string      `json:"token"`
	Error   bool        `json:"error"`
	User    string      `json:"user"`
}

type Response struct {
	Message interface{} `json:"message"`
	Error   bool        `json:"error"`
}

type SignupRequest struct {
	Username string `json:"username"`
	Password string `json:"password"`
	ImageUrl string `json:"imageUrl"`
}

type LoginRequest struct {
	Username string `json:"username"`
	Password string `json:"password"`
}

type AddGameRequest struct {
	Title       string  `json:"title"`
	Genre       string  `json:"genre"`
	ReleaseDate string  `json:"releaseDate"`
	Developer   string  `json:"developer"`
	Price       float64 `json:"price"`
	Stock       int     `json:"stock"`
	Description string  `json:"description"`
	ImageUrl    string  `json:"imageUrl"`
}

type AddReviewRequest struct {
	Username   string `json:"username"`
	GameTitle  string `json:"gameTitle"`
	ReviewText string `json:"reviewText"`
	Rating     int    `json:"rating"`
}

type AddReservationRequest struct {
	Username  string `json:"username"`
	GameTitle string `json:"gameTitle"`
	NumCopies int    `json:"numCopies"`
}

type AddPurchaseRequest struct {
	Username  string `json:"username"`
	GameTitle string `json:"gameTitle"`
	NumCopies int    `json:"numCopies"`
}

type UpdateUserRequest struct {
	Username    string `json:"username"`
	NewPassword string `json:"newPassword"`
	NewImageUrl string `json:"newImageUrl"`
}

type UpdateGameRequest struct {
	Title          string  `json:"title"`
	NewGenre       string  `json:"newGenre"`
	NewReleaseDate string  `json:"newReleaseDate"`
	NewDeveloper   string  `json:"newDeveloper"`
	NewPrice       float64 `json:"newPrice"`
	NewStock       int     `json:"newStock"`
	NewDescription string  `json:"newDescription"`
	NewImageUrl    string  `json:"newImageUrl"`
}

type UpdateReviewRequest struct {
	Username      string `json:"username"`
	GameTitle     string `json:"gameTitle"`
	NewReviewText string `json:"newReviewText"`
	NewRating     int    `json:"newRating"`
}

type UpdateReservationRequest struct {
	Username     string `json:"username"`
	GameTitle    string `json:"gameTitle"`
	NewNumCopies int    `json:"newNumCopies"`
}

type UpdatePurchaseRequest struct {
	NewNumCopies int    `json:"newNumCopies"`
	PurchaseID   string `json:"purchaseID"`
}

type DeleteReservationRequest struct {
	Username  string `json:"username"`
	GameTitle string `json:"gameTitle"`
}

type DeletePurchaseRequest struct {
	Username   string `json:"username"`
	GameTitle  string `json:"gameTitle"`
	PurchaseID string `json:"purchaseID"`
}

type DeleteReviewRequest struct {
	Username  string `json:"username"`
	GameTitle string `json:"gameTitle"`
}

type Notification struct {
	ID        int
	Message   string
	Timestamp time.Time
}

type NotificationRead struct {
	NotificationID int
	UserID         string
	Read           bool
}

type markNotificationAsReadRequest struct {
	NotificationID int
	Username       string
}

type User struct {
	ID struct {
		Oid string `json:"$oid"`
	} `json:"_id"`
	CreatedAt struct {
		Date int64 `json:"$date"`
	} `json:"created_at"`
	ImageURL     string        `json:"image_url"`
	Password     string        `json:"password"`
	Reservations []Reservation `json:"reservations"`
	Reviews      []Review      `json:"reviews"`
	Purchases    []Purchase    `json:"purchases"`
	Username     string        `json:"username"`
}

type Reservation struct {
	GameTitle       string `json:"game_title"`
	ReservationDate struct {
		Date int64 `json:"$date"`
	} `json:"reservation_date"`
	UserID struct {
		Oid string `json:"$oid"`
	} `json:"user_id"`
	Username string `json:"username"`
}

type Review struct {
	CreatedAt struct {
		Date int64 `json:"$date"`
	} `json:"created_at"`
	GameTitle  string `json:"game_title"`
	Rating     int    `json:"rating"`
	ReviewText string `json:"review_text"`
}

type Purchase struct {
	ID struct {
		Oid string `json:"$oid"`
	} `json:"_id"`
	GameTitle    string  `json:"game_title"`
	NumCopies    int     `json:"num_copies"`
	Price        float64 `json:"price"`
	PurchaseDate struct {
		Date int64 `json:"$date"`
	} `json:"purchase_date"`
	Username string `json:"username"`
}

type Game struct {
	Title       string    `json:"title"`
	Genre       string    `json:"genre"`
	ReleaseDate time.Time `json:"release_date"`
	Developer   string    `json:"developer"`
	Price       float64   `json:"price"`
	Stock       int       `json:"stock"`
	Description string    `json:"description"`
	ImageURL    string    `json:"image_url"`
	Reviews     struct{}  `json:"reviews"`
}
