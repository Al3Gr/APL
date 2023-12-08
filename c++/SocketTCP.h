#ifndef PROGETTO_SOCKETTCP_H
#define PROGETTO_SOCKETTCP_H

#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "iostream"
#include "exceptions/SocketException.h"

#pragma once

using namespace std;

class SocketTCP {
private:
    struct sockaddr_in address;
    int sockid;
public:
    SocketTCP(const char* address, int port) noexcept(false){
        this->address.sin_family = AF_INET;
        this->address.sin_port = htons(port);
        this->address.sin_addr.s_addr = inet_addr(address);
        this->sockid = socket(AF_INET, SOCK_STREAM, 0);
        if(this->sockid < -1){
            throw SocketException("Creation Error");
        }
    }
    void socketConnect() noexcept(false) {
        if(connect(this->sockid, (struct sockaddr*)&(this->address), sizeof(struct sockaddr_in)) < 0) {
            throw SocketException("Connection Error");
        }
    }

    void socketSend(const void* msg, size_t size) const noexcept(false){
        if(send(this->sockid, msg, size, 0) < 0){
            throw SocketException("Sending Error");
        }
    }


    void socketRecv(char* buffer, size_t size)  noexcept(false){
        if(recv(this->sockid, (void*)buffer, size, 0) < 0){
            throw SocketException("Receiving Error");
        }
    }

    // con memoria stack abbiamo deallocazione automatica dell'oggetto
    // con memoria heap no: devo invocare il distruttore
    // (delete/delete[] cancella l'area di memoria a cui fa riferimento il puntatore passato)
    ~SocketTCP(){
        //cout << "Chiuso socket" << endl;
        close(this->sockid);
    }


};


#endif //PROGETTO_SOCKETTCP_H
