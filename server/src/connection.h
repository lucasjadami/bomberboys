#ifndef CONNECTION_H
#define CONNECTION_H

#include "socket.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <map>

#define MAX_CONNECTIONS 100
#define SEED_SIZE       10000000
#define GHOST_PORT      2505

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

#define SERVER_COUNT    2
#define BIAS_SIZE       40
extern char serverNames[SERVER_COUNT][2][BIAS_SIZE];

class Connection
{
public:

                    Connection(int seed, void (*)(int, Socket*));
    virtual        ~Connection();
    void            init(int port);
	virtual	void    process() {};
    Socket*         getServerSocket();
    Socket*         getWorldServerSocket();
    void            connectToWorldServer(const char*, const char*);

protected:

    Socket*                         worldServerSocket;
    Socket*                         serverSocket;
	std::map<int, Socket*> 	        clientSockets;

    bool            isGhostServer(const char*, int);
    void            setTCPNoDelay(int);
    int             generateId();
    virtual int     createWorldServerSocket() { return -1; }
	virtual int     createServerSocket() { return -1; };
	void            (*connectionHandler)(int, Socket*);

private:

    int             idCount;
	// The seed is used to generate sockets IDs. Each server should have one seed, 0 <= seed <= 99.
	int             seed;

	sockaddr_in     bindSocket(int, int);

};

#endif
