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

#define ERRNO_CONNECTION_RESET 104

// MSG_NOSIGNAL does not exists on OS X.
#ifdef MAC_OS
#ifndef MSG_NOSIGNAL
#define MSG_NOSIGNAL SO_NOSIGPIPE
#endif
#endif

class Connection
{
public:

                    Connection(void (*)(int, Socket*));
    virtual        ~Connection();
    void            init(int port);
	virtual	void    process() {};
    Socket*         getServerSocket();

protected:

    Socket*                         serverSocket;
	std::map<int, Socket*> 	        clientSockets;
	int                             idCount;

	virtual int     create() { return -1; };
	void            (*connectionHandler)(int, Socket*);

};

#endif
