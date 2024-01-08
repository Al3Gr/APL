#ifndef PROGETTO_SOCKETTCP_H
#define PROGETTO_SOCKETTCP_H

#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "iostream"
#include "../exceptions/SocketException.h"

#pragma once

using namespace std;

class SocketTCP {
private:
    struct sockaddr_in address;
    int sockid;
public:
    SocketTCP(const char* address, int port) noexcept(false);
    void socketConnect() noexcept(false);
    void socketSend(const void* msg, size_t size) const noexcept(false);
    void socketRecv(char* buffer, size_t size)  noexcept(false);
    ~SocketTCP();


};


#endif //PROGETTO_SOCKETTCP_H
