#include "connection.h"
#include "output.h"
#include <cstring>

Connection::Connection()
{
        idCount = 0;
}

Connection::~Connection()
{
    if (serverSocket != NULL)
        delete serverSocket;
}

void Connection::init(int port)
{
    int serverFd = create();

    sockaddr_in address;

    memset(&address, 0, sizeof(address));
	address.sin_family = AF_INET;
	address.sin_port = htons(port);
	address.sin_addr.s_addr = INADDR_ANY;

	if (bind(serverFd, (sockaddr*) &address, sizeof(address)) < 0)
  		error("ERROR on binding");

    serverSocket = new Socket(serverFd, address);

    listen(serverFd, MAX_CONNECTIONS);
}

