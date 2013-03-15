#ifndef CONNECTION_H
#define CONNECTION_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <map>

class Connection
{
public:

	virtual void                create() {};
            void                init(int port);
	virtual	void                process() {};

protected:

	sockaddr_in 			    serverSocket;
	int 		    		    serverFd;
	std::map<int, sockaddr_in> 	clientSockets;
	char				        buffer[256];

};

#endif
