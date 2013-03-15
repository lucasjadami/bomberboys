#ifndef CONNECTION_H
#define CONNECTION_H

#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

class Connection
{
public:

	virtual void create() {};
		void init(int port);
	virtual	void process() {};

protected:

	sockaddr_in 	serverSocket;
	int 		socketFd;

};

#endif
