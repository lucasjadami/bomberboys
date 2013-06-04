#include "connection.h"
#include "output.h"
#include <cstring>
#include <unistd.h>
#include <netinet/tcp.h>

char serverNames[SERVER_COUNT][2][BIAS_SIZE] =
{
    {
        "127.0.0.1", "10011"
    },
    {
        "127.0.0.1", "10012"
    }
};

Connection::Connection(int seed, void (*connectionHandler)(int, Socket*))
{
    worldServerSocket = NULL;
    this->seed = seed;
    this->connectionHandler = connectionHandler;
    idCount = 0;
}

Connection::~Connection()
{
    if (worldServerSocket != NULL)
    {
        close(worldServerSocket->getFd());
        delete worldServerSocket;
    }

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
    int serverFd = createServerSocket();

    sockaddr_in address = bindSocket(serverFd, port);
    serverSocket = new Socket(-1, serverFd, address);

    listen(serverFd, MAX_CONNECTIONS);
}

Socket* Connection::getServerSocket()
{
    return serverSocket;
}

Socket* Connection::getWorldServerSocket()
{
    return worldServerSocket;
}

void Connection::connectToWorldServer(const char* worldServerName, const char* port)
{
    int serverFd = createWorldServerSocket();

    bindSocket(serverFd, GHOST_PORT);

    addrinfo hints, *addressInfo;
    int status;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    if ((status = getaddrinfo(worldServerName, port, &hints, &addressInfo)) != 0)
        error("Error on get server info, %s", gai_strerror(status));

    connect(serverFd, addressInfo->ai_addr, addressInfo->ai_addrlen);

    worldServerSocket = new Socket(generateId(), serverFd, addressInfo);
}

bool Connection::isGhostServer(const char* serverName, int port)
{
    if (port != GHOST_PORT)
        return false;

    for (int i = 0; i < SERVER_COUNT; ++i)
    {
        if (strcmp(serverName, serverNames[i][0]) == 0)
            return true;
    }

    return false;
}

void Connection::setTCPNoDelay(int fd)
{
    int flag = 1;
    if (setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (char*) &flag, sizeof(int)) < 0)
        error("ERROR on TCP_NODELAY, %s", strerror(errno));
}

int Connection::generateId()
{
    return seed * SEED_SIZE + idCount++;
}

sockaddr_in Connection::bindSocket(int fd, int port)
{
    int yes = 1;
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
        error("ERROR on set SO_REUSEADDR ,%s", strerror(errno));

    sockaddr_in address;

    memset(&address, 0, sizeof(address));
	address.sin_family = AF_INET;
	address.sin_port = htons(port);
	address.sin_addr.s_addr = INADDR_ANY;

	if (bind(fd, (sockaddr*) &address, sizeof(address)) < 0)
  		error("ERROR on binding, %s", strerror(errno));

    return address;
}
