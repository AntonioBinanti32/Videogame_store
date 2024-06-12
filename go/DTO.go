package main

type Response struct {
	Message string `json:"message"`
	Token   string `json:"token"`
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
	GameID    string `json:"gameID"`
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
	Username     string `json:"username"`
	GameTitle    string `json:"gameTitle"`
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

//TODO: Implementare altri DTO
