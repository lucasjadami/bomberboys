#include "connection.h"
#include "output.h"
#include <cstring>
#include <unistd.h>

Connection::Connection(void (*connectionHandler)(int, Socket*))
{
    this->connectionHandler = connectionHandler;
    idCount = 0;
}

Connection::~Connection()
{
    std::map<int, Socket*>::iterator it = clientSockets.begin();
    while (it != clientSockets.end())
    {
        shutdown(it->second->getFd(), SHUT_RDWR);
        delete it->second;
        it++;
    }

    if (serverSocket != NULL)
    {
        close(serverSocket->getFd());
        delete serverSocket;
    }
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

    serverSocket = new Socket(-1, serverFd, address);

    listen(serverFd, MAX_CONNECTIONS);
}

