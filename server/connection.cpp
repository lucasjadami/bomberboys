#include "connection.h"
#include "output.h"
#include <cstring>

void Connection::init(int port)
{
	memset(&serverSocket, 0, sizeof(serverSocket));
	serverSocket.sin_family = AF_INET;
	serverSocket.sin_port = htons(port);
	serverSocket.sin_addr.s_addr = INADDR_ANY;
	
	if (bind(socketFd, (sockaddr*) &serverSocket, sizeof(serverSocket)) < 0)
  		error("ERROR on binding");
}

