#include "non_blocking_tcp_connection.h"
#include "output.h"
#include <fcntl.h>
#include <cstdlib>
#include <utility>
#include <unistd.h>

void NonBlockingTcpConnection::create()
{
	serverFd = socket(AF_INET, SOCK_STREAM, 0);
    if (serverFd < 0)
        error("ERROR opening socket");

    maxFd = serverFd;

    fcntl(serverFd, F_SETFL, O_NONBLOCK);
}

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

	if (FD_ISSET(serverFd, &readFdSet))
        getNewClient();
}

void NonBlockingTcpConnection::getNewClient()
{
	sockaddr_in clientSocket;
	socklen_t socketLen = sizeof(clientSocket);
	int clientFd = accept(serverFd, (sockaddr*) &clientSocket, &socketLen);
	if (clientFd < 0)
		error("ERROR on accept");

	maxFd = clientFd > maxFd ? clientFd : maxFd;
	clientSockets.insert(std::make_pair(clientFd, clientSocket));

	debug("New connection\n");
}

void NonBlockingTcpConnection::processClients(fd_set& readFdSet)
{
	std::map<int, sockaddr_in>::iterator it = clientSockets.begin();
    while (it != clientSockets.end())
    {
        bool removeIt = false;
        if (FD_ISSET(it->first, &readFdSet))
        {
            int bytesRead;
            if ((bytesRead = recv(it->first, buffer, sizeof buffer, 0)) <= 0)
            {
                if (bytesRead == 0)
                    removeIt = true;
                else
                    error("ERROR on recv");
                close(it->first);
                FD_CLR(it->first, &readFdSet);

                debug("Connection closed\n");
            }
            else
            {
                debug("%s\n", buffer);
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
    FD_SET(serverFd, &fdSet);
    std::map<int, sockaddr_in>::iterator it = clientSockets.begin();
    while (it != clientSockets.end())
    {
        FD_SET(it->first, &fdSet);
        it++;
    }
}
