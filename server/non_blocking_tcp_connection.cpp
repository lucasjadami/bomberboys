#include "non_blocking_tcp_connection.h"
#include "output.h"
#include <fcntl.h>
#include <cstdlib>

void NonBlockingTcpConnection::create()
{
	socketFd = socket(AF_INET, SOCK_STREAM, 0);
     	if (socketFd < 0) 
        	error("ERROR opening socket");
        
        fcntl(socketFd, F_SETFL, O_NONBLOCK);
        
        FD_ZERO(&readFdSet);
    	FD_SET(socketFd, &readFdSet);
    	
    	selectTimeout.tv_usec = 1000;
}

void NonBlockingTcpConnection::process()
{
	listen(socketFd, 5);
	
	if (select(socketFd + 1, &readFdSet, NULL, NULL, &selectTimeout) < 0)
		error("ERROR on select");
		
	if (FD_ISSET(socketFd, &readFdSet))
        {
        	// TODO: new clients.
        }
}
