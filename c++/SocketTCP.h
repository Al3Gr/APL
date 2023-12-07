//
// Created by Alessandro Gravagno on 07/12/23.
//

#ifndef PROGETTO_SOCKETTCP_H
#define PROGETTO_SOCKETTCP_H


#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "iostream"
#include "personal_exception/SocketException.h"

using namespace std;

class SocketTCP {
private:
    struct sockaddr_in address{};
    int sockid;
public:
    SocketTCP(const char* address, int port) throw(SocketException);
    void socketConnect() throw (SocketException);
    void socketSend();
    void socketRecv();


};


#endif //PROGETTO_SOCKETTCP_H
