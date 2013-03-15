#include "socket.h"

Socket::Socket(int fd, sockaddr_in address)
{
    this->fd = fd;
    this->address = address;
}

sockaddr_in Socket::getAddress()
{
    return address;
}

int Socket::getFd()
{
    return fd;
}
