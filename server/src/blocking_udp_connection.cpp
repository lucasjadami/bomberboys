#include "blocking_udp_connection.h"
#include "output.h"
#include <fcntl.h>
#include <cstdlib>
#include <utility>
#include <unistd.h>
#include <arpa/inet.h>
#include <utility>

static void* mainThread(void* threadPtr)
{
    Thread* thread = (Thread*) threadPtr;
    BlockingUdpConnection* connection = (BlockingUdpConnection*) thread->connection;

    while (thread->running)
        connection->readFromClient();

    pthread_exit(NULL);
}

static void* clientSendThread(void* threadPtr)
{
    Thread* thread = (Thread*) threadPtr;
    BlockingUdpConnection* connection = (BlockingUdpConnection*) thread->connection;
    Socket* socket = thread->socket;
    sockaddr_in address = socket->getAddress();
    socklen_t addressLen = sizeof(address);
    int bytesWritten = 0;

    while (thread->running)
    {
        // Send is blocking but it can write 0 bytes at every loop iteration.
        usleep(1000);

        if (socket->getOutBufferSize() > 0)
        {
            bytesWritten = sendto(connection->getServerSocket()->getFd(), socket->getOutBuffer(),
                                  socket->getOutBufferSize(), MSG_NOSIGNAL, (sockaddr*) &address, addressLen);
        }

        pthread_mutex_lock(&thread->mutex);

        // The server waits til the output buffer is empty to disconnect the client.
        if (socket->isDisconnectForced())
        {
            debug("Forced connection shutdown from %s", inet_ntoa(socket->getAddress().sin_addr));
            connection->disconnectClientAndStopThread(thread);
            pthread_exit(NULL);
        }

        if (!thread->running)
            continue;
        else if (bytesWritten < 0)
        {
            if (errno == ERRNO_CONNECTION_RESET)
                debug("Connection closed from %s", inet_ntoa(socket->getAddress().sin_addr));
            else
                warning("ERROR on send, %s", strerror(errno));
            connection->disconnectClientAndStopThread(thread);
            pthread_exit(NULL);
        }
        while (socket->updateOutBuffer(bytesWritten));
        pthread_mutex_unlock(&thread->mutex);
    }

    pthread_exit(NULL);
}

BlockingUdpConnection::BlockingUdpConnection(void (*connectionHandler)(int, Socket*))
    : Connection(connectionHandler)
{
    connectionLaunched = false;
    addressIdentifierCount = 0;
}

BlockingUdpConnection::~BlockingUdpConnection()
{
    serverThread.running = false;

    if (serverSocket != NULL)
        shutdown(serverSocket->getFd(), SHUT_RDWR);

    pthread_join(serverThread.pthreadAccept, NULL);

    for (std::map<int, Thread*>::iterator it = clientThreads.begin(); it != clientThreads.end(); ++it)
    {
        Thread* thread = it->second;

        pthread_join(thread->pthreadSend, NULL);
        pthread_mutex_destroy(&thread->mutex);
        delete thread;
    }
}

void BlockingUdpConnection::process()
{
    if (!connectionLaunched)
    {
        connectionLaunched = true;

        serverThread.running = true;
        serverThread.socket = serverSocket;
        serverThread.connection = this;

        if (pthread_create(&serverThread.pthreadAccept, NULL, mainThread, &serverThread) != 0)
            error("ERROR creating thread, %s", strerror(errno));
    }
}

int BlockingUdpConnection::create()
{
	int serverFd = socket(AF_INET, SOCK_DGRAM, 0);
    if (serverFd < 0)
        error("ERROR opening socket, %s", strerror(errno));

    return serverFd;
}

void BlockingUdpConnection::readFromClient()
{
	sockaddr_in address;
	socklen_t addressLen = sizeof(address);
    char buffer[BUFFER_SIZE];

    int bytesRead;
    if ((bytesRead = recvfrom(serverSocket->getFd(), buffer, sizeof(buffer), MSG_NOSIGNAL,
                              (sockaddr*) &address, &addressLen)) < 0)
    {
        warning("ERROR on UDP accept, %s", strerror(errno));
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

        Thread* thread = new Thread();
        thread->socket = socket;
        thread->connection = this;
        thread->running = true;

        pthread_mutex_init(&thread->mutex, NULL);

        clientThreads.insert(std::make_pair(socket->getId(), thread));

        if (pthread_create(&thread->pthreadSend, NULL, clientSendThread, thread) != 0)
            error("ERROR creating thread, %s", strerror(errno));

        connectionHandler(EVENT_CLIENT_CONNECTED, socket);

        debug("New connection from %s:%d", inet_ntoa(address.sin_addr), ntohs(address.sin_port));
    }
    else
        socket = clientSockets[clientDescriptors[clientFd]];

    Thread* thread = clientThreads[socket->getId()];
    pthread_mutex_lock(&thread->mutex);
    if (thread->running)
    {
        if (bytesRead <= 0)
        {
            if (bytesRead == 0)
                debug("Connection closed from %s", inet_ntoa(socket->getAddress().sin_addr));
            else
                warning("ERROR on recv %s", strerror(errno));

            disconnectClientAndStopThread(thread);
        }
        else
        {
            socket->appendInBuffer(buffer, bytesRead);
            while (socket->updateInBuffer(bytesRead));
        }
    }
    pthread_mutex_unlock(&thread->mutex);
}

void BlockingUdpConnection::disconnectClientAndStopThread(Thread* thread)
{
    Socket* socket = thread->socket;

    connectionHandler(EVENT_CLIENT_DISCONNECTED, socket);

    thread->running = false;
    pthread_mutex_unlock(&thread->mutex);
}
