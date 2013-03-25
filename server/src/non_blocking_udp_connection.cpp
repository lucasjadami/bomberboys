#include "non_blocking_udp_connection.h"
#include "output.h"
#include <fcntl.h>
#include <cstdlib>
#include <utility>
#include <unistd.h>
#include <arpa/inet.h>
#include <utility>

NonBlockingUdpConnection::NonBlockingUdpConnection(void (*connectionHandler)(int, Socket*))
    : Connection(connectionHandler)
{
    addressIdentifierCount = 0;
}

void NonBlockingUdpConnection::process()
{
    // The select operation changes this structure. It must be reinitialized allways. See 'man' page.
    selectTimeout.tv_sec = 0;
    selectTimeout.tv_usec = 1000;

    fd_set readFdSet;
    fd_set writeFdSet;
    createFdSet(readFdSet);
    writeFdSet = readFdSet;

    if (select(serverSocket->getFd() + 1, &readFdSet, &writeFdSet, NULL, &selectTimeout) < 0)
		error("ERROR on select");

	if (FD_ISSET(serverSocket->getFd(), &readFdSet))
        readClient();
    if (FD_ISSET(serverSocket->getFd(), &writeFdSet))
        writeClients();
}

int NonBlockingUdpConnection::create()
{
	int serverFd = socket(AF_INET, SOCK_DGRAM, 0);
    if (serverFd < 0)
        error("ERROR opening socket");

    fcntl(serverFd, F_SETFL, O_NONBLOCK);

    return serverFd;
}

void NonBlockingUdpConnection::readClient()
{
	sockaddr_in address;
	socklen_t addressLen = sizeof(address);
    char buffer[BUFFER_SIZE];

    int bytesRead;
    if ((bytesRead = recvfrom(serverSocket->getFd(), buffer, sizeof(buffer), 0,
                              (sockaddr*) &address, &addressLen)) < 0)
    {
        warning("ERROR on UDP accept");
        return;
    }

    if (addressIdentifiers.find(address.sin_addr.s_addr) == addressIdentifiers.end())
        addressIdentifiers.insert(std::make_pair(addressIdentifierCount++, address.sin_addr.s_addr));

    int clientFd = addressIdentifiers[address.sin_addr.s_addr];
    clientFd = clientFd << 16;
    clientFd += ntohs(address.sin_port);

    Socket* socket;
    if (clientDescriptors.find(clientFd) == clientDescriptors.end())
    {
        clientDescriptors.insert(std::make_pair(clientFd, idCount));

        socket = new Socket(idCount, clientFd, address);
        clientSockets.insert(std::make_pair(idCount, socket));
        idCount++;

        connectionHandler(EVENT_CLIENT_CONNECTED, socket);

        debug("New connection from %s:%d", inet_ntoa(address.sin_addr), ntohs(address.sin_port));
    }
    else
        socket = clientSockets[clientDescriptors[clientFd]];


    if (bytesRead <= 0)
    {
        if (bytesRead == 0)
            debug("Connection closed from %s", inet_ntoa(socket->getAddress().sin_addr));
        else
            warning("ERROR on recv %s", strerror(errno));

        delete socket;
        clientSockets.erase(clientDescriptors[clientFd]);
        clientDescriptors.erase(clientFd);
    }
    else
    {
        socket->appendInBuffer(buffer, bytesRead);
        socket->updateInBuffer(bytesRead);
    }
}

void NonBlockingUdpConnection::writeClients()
{
    std::map<int, Socket*>::iterator it = clientSockets.begin();
    while (it != clientSockets.end())
    {
        bool removeIt = false;
        Socket* socket = it->second;

        sockaddr_in address = socket->getAddress();
        socklen_t addressLen = sizeof(address);

        int bytesWritten;
        if ((bytesWritten = sendto(serverSocket->getFd(), socket->getOutBuffer(), socket->getOutBufferSize(), 0,
                                   (sockaddr*) &address, addressLen)) < 0)
        {
            warning("ERROR on send %s", strerror(errno));
            removeIt = true;
            close(socket->getFd());
        }
        else
        {
            socket->updateOutBuffer(bytesWritten);
        }

        if (removeIt)
        {
            connectionHandler(EVENT_CLIENT_DISCONNECTED, socket);
            delete socket;
            clientSockets.erase(it++);
        }
        else
            it++;
    }
}

void NonBlockingUdpConnection::createFdSet(fd_set& fdSet)
{
    FD_ZERO(&fdSet);
    FD_SET(serverSocket->getFd(), &fdSet);
}
