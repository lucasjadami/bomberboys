#include "non_blocking_tcp_connection.h"
#include "output.h"
#include <fcntl.h>
#include <cstdlib>
#include <utility>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>

NonBlockingTcpConnection::NonBlockingTcpConnection(int seed, void (*connectionHandler)(int, Socket*))
    : Connection(seed, connectionHandler)
{
}

void NonBlockingTcpConnection::process()
{
    // The select operation changes this structure. It must be reinitialized allways. See 'man' page.
    selectTimeout.tv_sec = 0;
    selectTimeout.tv_usec = 1000;

    fd_set readFdSet;
    fd_set writeFdSet;
    createFdSet(readFdSet);
    writeFdSet = readFdSet;

    if (select(maxFd + 1, &readFdSet, &writeFdSet, NULL, &selectTimeout) < 0)
		error("ERROR on select, %s", strerror(errno));

	processClients(readFdSet, writeFdSet);

	if (FD_ISSET(serverSocket->getFd(), &readFdSet))
        getNewClient();
}

int NonBlockingTcpConnection::create()
{
	int serverFd = socket(AF_INET, SOCK_STREAM, 0);
    if (serverFd < 0)
        error("ERROR opening socket, %s", strerror(errno));

    maxFd = serverFd;

    fcntl(serverFd, F_SETFL, O_NONBLOCK);

    return serverFd;
}

void NonBlockingTcpConnection::getNewClient()
{
	sockaddr_in address;
	socklen_t addressLen = sizeof(address);
	int clientFd = accept(serverSocket->getFd(), (sockaddr*) &address, &addressLen);
	if (clientFd < 0)
		error("ERROR on accept, %s", strerror(errno));

    int flag = 1;
    if (setsockopt(clientFd, IPPROTO_TCP, TCP_NODELAY, (char*) &flag, sizeof(int)) < 0)
        error("ERROR on TCP_NODELAY, %s", strerror(errno));

	maxFd = clientFd > maxFd ? clientFd : maxFd;

    int socketId = generateId();
	Socket* socket = new Socket(socketId, clientFd, address);
	clientSockets.insert(std::make_pair(socketId, socket));

    connectionHandler(EVENT_CLIENT_CONNECTED, socket);

	debug("New connection from %s", inet_ntoa(address.sin_addr));
}

void NonBlockingTcpConnection::processClients(fd_set& readFdSet, fd_set& writeFdSet)
{
	std::map<int, Socket*>::iterator it = clientSockets.begin();
    while (it != clientSockets.end())
    {
        bool removeIt = false;
        Socket* socket = it->second;

        if (socket->isDisconnectForced())
        {
            debug("Forced connection shutdown from %s", inet_ntoa(socket->getAddress().sin_addr));
            removeIt = true;
            close(socket->getFd());
        }

        if (!removeIt && FD_ISSET(socket->getFd(), &readFdSet))
        {
            int bytesRead;
            if ((bytesRead = recv(socket->getFd(), socket->getInBuffer(), socket->getInBufferSize(), 0)) <= 0)
            {
                if (bytesRead == 0)
                    debug("Connection closed from %s", inet_ntoa(socket->getAddress().sin_addr));
                else
                    warning("ERROR on recv, %s", strerror(errno));
                removeIt = true;
                close(socket->getFd());
            }
            else
            {
                while (socket->updateInBuffer(bytesRead));
            }
        }

        if (!removeIt && FD_ISSET(socket->getFd(), &writeFdSet))
        {
            int bytesWritten;
            if ((bytesWritten = send(socket->getFd(), socket->getOutBuffer(), socket->getOutBufferSize(), 0)) < 0)
            {
                if (errno == ERRNO_CONNECTION_RESET)
                    debug("Connection closed from %s", inet_ntoa(socket->getAddress().sin_addr));
                else
                    warning("ERROR on send, %s", strerror(errno));
                removeIt = true;
                close(socket->getFd());
            }
            else
            {
                while (socket->updateOutBuffer(bytesWritten));
            }
        }

        if (removeIt)
        {
            connectionHandler(EVENT_CLIENT_DISCONNECTED, socket);
            clientSockets.erase(it++);
            delete socket;
        }
        else
            it++;
    }
}

void NonBlockingTcpConnection::createFdSet(fd_set& fdSet)
{
    FD_ZERO(&fdSet);
    FD_SET(serverSocket->getFd(), &fdSet);

    std::map<int, Socket*>::iterator it = clientSockets.begin();
    while (it != clientSockets.end())
    {
        FD_SET(it->second->getFd(), &fdSet);
        it++;
    }
}
