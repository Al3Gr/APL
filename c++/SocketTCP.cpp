//
// Created by Alessandro Gravagno on 07/12/23.
//

#include "SocketTCP.h"

SocketTCP::SocketTCP(const char *address, int port) throw(SocketException){
    this->address.sin_family = AF_INET;
    this->address.sin_port = htons(port);
    this->address.sin_addr.s_addr = inet_addr(address);
    this->sockid = socket(AF_INET, SOCK_STREAM, 0);
    if(this->sockid < -1){
        throw SocketException("Creation Error");
    }
}

void SocketTCP::socketConnect() throw(SocketException) {
    if(connect(this->sockid, (struct sockaddr*)&(this->address), sizeof(struct sockaddr_in)) < 0){
        throw SocketException("Connection Error");
    }
}


