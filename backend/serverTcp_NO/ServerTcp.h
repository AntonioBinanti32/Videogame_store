#ifndef VIDEOGAME_STORE_SERVERTCP_H
#define VIDEOGAME_STORE_SERVERTCP_H

/*#include <stdexcept>
#include <winsock2.h>
#include <WS2tcpip.h>*/


#pragma comment(lib, "ws2_32.lib")

class SocketException : public std::runtime_error {
public:
    explicit SocketException(const std::string& message);
};

class SocketTCPServer {
public:
    SocketTCPServer(int port);
    ~SocketTCPServer();
    int acceptConnection(struct sockaddr_in& client_address);

private:
    SOCKET server_sockid;
    struct sockaddr_in server_address;
};

#endif // VIDEOGAME_STORE_SERVERTCP_H
