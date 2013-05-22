#include "connection.h"
#include "output.h"
#include <cstring>
#include <unistd.h>

Connection::Connection(int seed, void (*connectionHandler)(int, Socket*))
{
    this->seed = seed;
    this->connectionHandler = connectionHandler;
    idCount = 0;
}

Connection::~Connection()
{
    std::map<int, Socket*>::iterator it = clientSockets.begin();
    while (it != clientSockets.end())
    {
        close(it->second->getFd());
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

    int yes = 1;
    if (setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
        error("ERROR on set SO_REUSEADDR %s", strerror(errno));

    sockaddr_in address;

    memset(&address, 0, sizeof(address));
	address.sin_family = AF_INET;
	address.sin_port = htons(port);
	address.sin_addr.s_addr = INADDR_ANY;

	if (bind(serverFd, (sockaddr*) &address, sizeof(address)) < 0)
  		error("ERROR on binding %s", strerror(errno));

    serverSocket = new Socket(-1, serverFd, address);

    listen(serverFd, MAX_CONNECTIONS);
}

Socket* Connection::getServerSocket()
{
    return serverSocket;
}

int Connection::generateId()
{
    return seed * SEED_SIZE + idCount++;
}
