#ifndef CONNECTION_H
#define CONNECTION_H

#include "socket.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <map>

#define MAX_CONNECTIONS 100
#define SEED_SIZE       10000000

#define EVENT_CLIENT_CONNECTED      0
#define EVENT_CLIENT_DISCONNECTED   1
#define EVENT_SERVER_CONNECTED      2
#define EVENT_SERVER_DISCONNECTED   3

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

                    Connection(int seed, void (*)(int, Socket*));
    virtual        ~Connection();
    void            init(int port);
	virtual	void    process() {};
    Socket*         getServerSocket();

protected:

    Socket*                         serverSocket;
	std::map<int, Socket*> 	        clientSockets;

    int             generateId();
	virtual int     create() { return -1; };
	void            (*connectionHandler)(int, Socket*);

private:

    int                             idCount;
	// The seed is used to generate sockets IDs. Each server should have one seed, 0 <= seed <= 99.
	int                             seed;

};

#endif
