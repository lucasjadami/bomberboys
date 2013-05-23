#include "server.h"

Server::Server(Socket* socket)
{
    this->socket = socket;
}

Socket* Server::getSocket()
{
    return socket;
}
