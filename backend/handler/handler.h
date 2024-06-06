#ifndef VIDEOGAME_STORE_HANDLER_H
#define VIDEOGAME_STORE_HANDLER_H

#include "../socketTcp/SocketTcp.h"

namespace handler {
	void handleClient(SocketTcp& serverSocket, SOCKET clientSocker);
	void handleLogin(const std::string& message, SocketTcp& serverSocket, SOCKET clientSocket);
	void handleSignup(const std::string& message, SocketTcp& serverSocket, SOCKET clientSocket);
	//TODO: Implementare handler per altri metodi
};

#endif // VIDEOGAME_STORE_HANDLER_H
