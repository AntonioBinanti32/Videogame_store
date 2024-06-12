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
	ActualUser  string  `json:"actualUser"`
	Token       string  `json:"token"`
}

type AddReviewRequest struct {
	Username   string `json:"username"`
	GameTitle  string `json:"gameTitle"`
	ReviewText string `json:"reviewText"`
	Rating     int    `json:"rating"`
	ActualUser string `json:"actualUser"`
	Token      string `json:"token"`
}

type AddReservationRequest struct {
	Username   string `json:"username"`
	GameID     string `json:"gameID"`
	NumCopies  int    `json:"numCopies"`
	ActualUser string `json:"actualUser"`
	Token      string `json:"token"`
}

type AddPurchaseRequest struct {
	Username   string `json:"username"`
	GameTitle  string `json:"gameTitle"`
	NumCopies  int    `json:"numCopies"`
	ActualUser string `json:"actualUser"`
	Token      string `json:"token"`
}

type UpdateUserRequest struct {
	Username    string `json:"username"`
	NewPassword string `json:"newPassword"`
	NewImageUrl string `json:"newImageUrl"`
	ActualUser  string `json:"actualUser"`
	Token       string `json:"token"`
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
	ActualUser     string  `json:"actualUser"`
	Token          string  `json:"token"`
}

type UpdateReviewRequest struct {
	Username      string `json:"username"`
	GameTitle     string `json:"gameTitle"`
	NewReviewText string `json:"newReviewText"`
	NewRating     int    `json:"newRating"`
	ActualUser    string `json:"actualUser"`
	Token         string `json:"token"`
}

type UpdateReservationRequest struct {
	Username     string `json:"username"`
	GameTitle    string `json:"gameTitle"`
	NewNumCopies int    `json:"newNumCopies"`
	ActualUser   string `json:"actualUser"`
	Token        string `json:"token"`
}

type UpdatePurchaseRequest struct {
	Username     string `json:"username"`
	GameTitle    string `json:"gameTitle"`
	NewNumCopies int    `json:"newNumCopies"`
	PurchaseID   string `json:"purchaseID"`
	ActualUser   string `json:"actualUser"`
	Token        string `json:"token"`
}

type DeleteReservationRequest struct {
	Username   string `json:"username"`
	GameTitle  string `json:"gameTitle"`
	ActualUser string `json:"actualUser"`
	Token      string `json:"token"`
}

type DeletePurchaseRequest struct {
	Username   string `json:"username"`
	GameTitle  string `json:"gameTitle"`
	PurchaseID string `json:"purchaseID"`
	ActualUser string `json:"actualUser"`
	Token      string `json:"token"`
}

type DeleteReviewRequest struct {
	Username   string `json:"username"`
	GameTitle  string `json:"gameTitle"`
	ActualUser string `json:"actualUser"`
	Token      string `json:"token"`
}

type AuthenticationData struct {
	ActualUser string `json:"actualUser"`
	Token      string `json:"token"`
}

//TODO: Implementare altri DTO
