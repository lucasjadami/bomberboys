#ifndef SERVER_H
#define SERVER_H

#include "socket.h"

class Server
{
public:

            Server(Socket*);
    Socket* getSocket();

private:

    Socket* socket;

};

#endif
