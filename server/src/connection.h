#ifndef CONNECTION_H
#define CONNECTION_H

#include "socket.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <map>

#define MAX_CONNECTIONS 100

#define EVENT_CLIENT_CONNECTED      0
#define EVENT_CLIENT_DISCONNECTED   1

class Connection
{
public:

                    Connection(void (*)(int, Socket*));
    virtual        ~Connection();
            void    init(int port);
	virtual	void    process() {};

protected:

    Socket*                 serverSocket;
	std::map<int, Socket*> 	clientSockets;
	unsigned int            idCount;

	virtual int     create() { return -1; };
	void            (*connectionHandler)(int, Socket*);

};

#endif
