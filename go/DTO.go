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

//TODO: Implementare altri DTO
