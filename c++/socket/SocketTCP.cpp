#include "SocketTCP.h"

SocketTCP::SocketTCP(const char *address, int port) noexcept(false) {
    this->address.sin_family = AF_INET;
    this->address.sin_port = htons(port);
    this->address.sin_addr.s_addr = inet_addr(address);
    this->sockid = socket(AF_INET, SOCK_STREAM, 0);
    if(this->sockid < -1){
        throw SocketException("Creation Error");
    }
}

void SocketTCP::socketConnect() noexcept(false) {
    if(connect(this->sockid, (struct sockaddr*)&(this->address), sizeof(struct sockaddr_in)) < 0) {
        throw SocketException("Connection Error");
    }
}

void SocketTCP::socketSend(const void *msg, size_t size) const noexcept(false) {
    if(send(this->sockid, msg, size, 0) < 0){
        throw SocketException("Sending Error");
    }
}

void SocketTCP::socketRecv(char *buffer, size_t size) noexcept(false) {
    if(recv(this->sockid, (void*)buffer, size, 0) < 0){
        throw SocketException("Receiving Error");
    }
}

SocketTCP::~SocketTCP(){
    cout << "Chiuso socket" << endl;
    close(this->sockid);
}