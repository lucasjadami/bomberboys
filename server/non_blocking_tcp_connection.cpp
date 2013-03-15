#include "non_blocking_tcp_connection.h"
#include "output.h"
#include <fcntl.h>
#include <cstdlib>
#include <utility>
#include <unistd.h>

void NonBlockingTcpConnection::process()
{
    // The select operation changes this structure. It must be reinitialized allways. See 'man' page.
    selectTimeout.tv_sec = 0;
    selectTimeout.tv_usec = 1000;

    fd_set readFdSet;
    createFdSet(readFdSet);

    if (select(maxFd + 1, &readFdSet, NULL, NULL, &selectTimeout) < 0)
		error("ERROR on select");

	processClients(readFdSet);

	if (FD_ISSET(serverSocket->getFd(), &readFdSet))
        getNewClient();
}

int NonBlockingTcpConnection::create()
{
	int serverFd = socket(AF_INET, SOCK_STREAM, 0);
    if (serverFd < 0)
        error("ERROR opening socket");

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
		error("ERROR on accept");

	maxFd = clientFd > maxFd ? clientFd : maxFd;
	clientSockets.insert(std::make_pair(idCount++, new Socket(clientFd, address)));

	debug("New connection");
}

void NonBlockingTcpConnection::processClients(fd_set& readFdSet)
{
	std::map<int, Socket*>::iterator it = clientSockets.begin();
    while (it != clientSockets.end())
    {
        bool removeIt = false;
        Socket* socket = it->second;
        if (FD_ISSET(socket->getFd(), &readFdSet))
        {
            int bytesRead;
            if ((bytesRead = recv(socket->getFd(), socket->getInBuffer(), sizeof(socket->getInBuffer()), 0)) <= 0)
            {
                removeIt = true;
                if (bytesRead == 0)
                    debug("Connection closed");
                else
                    error("ERROR on recv");
                close(socket->getFd());
            }
            else
            {
                socket->updateInBuffer(bytesRead);
            }
        }

        if (removeIt)
            clientSockets.erase(it++);
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
