#include "blocking_tcp_connection.h"
#include "output.h"
#include <cstdlib>
#include <unistd.h>
#include <arpa/inet.h>

static void* mainThread(void* threadPtr)
{
    Thread* thread = (Thread*) threadPtr;
    BlockingTcpConnection* connection = (BlockingTcpConnection*) thread->connection;

    while (thread->running)
        connection->getNewClient();

    pthread_exit(NULL);
}

static void* clientSendThread(void* threadPtr)
{
    Thread* thread = (Thread*) threadPtr;
    BlockingTcpConnection* connection = (BlockingTcpConnection*) thread->connection;
    Socket* socket = thread->socket;
    int bytesWritten = 0;

    while (thread->running)
    {
        // Send is blocking but it can write 0 bytes at every loop iteration.
        usleep(1000);
        bytesWritten = send(socket->getFd(), socket->getOutBuffer(), socket->getOutBufferSize(), MSG_NOSIGNAL);

        pthread_mutex_lock(&thread->mutex);

        // The server waits til the output buffer is empty to disconnect the client.
        if (socket->isDisconnectForced())
        {
            debug("Forced connection shutdown from %s", inet_ntoa(socket->getAddress().sin_addr));
            connection->disconnectClientAndKillThread(thread);
        }

        if (!thread->running)
            continue;
        else if (bytesWritten < 0)
        {
            if (errno == ERRNO_CONNECTION_RESET)
                debug("Connection closed from %s", inet_ntoa(socket->getAddress().sin_addr));
            else
                warning("ERROR on send, %s", strerror(errno));
            connection->disconnectClientAndKillThread(thread);
        }
        while (socket->updateOutBuffer(bytesWritten));
        pthread_mutex_unlock(&thread->mutex);
    }

    pthread_exit(NULL);
}

static void* clientRecvThread(void* threadPtr)
{
    Thread* thread = (Thread*) threadPtr;
    BlockingTcpConnection* connection = (BlockingTcpConnection*) thread->connection;
    Socket* socket = thread->socket;
    int bytesRead = 0;

    while (thread->running)
    {
        bytesRead = recv(socket->getFd(), socket->getInBuffer(), socket->getInBufferSize(), MSG_NOSIGNAL);

        pthread_mutex_lock(&thread->mutex);
        if (!thread->running)
            continue;
        else if (bytesRead <= 0)
        {
            if (bytesRead == 0)
                debug("Connection closed from %s", inet_ntoa(socket->getAddress().sin_addr));
            else
                warning("ERROR on recv, %s", strerror(errno));
            connection->disconnectClientAndKillThread(thread);
        }
        while (socket->updateInBuffer(bytesRead));
        pthread_mutex_unlock(&thread->mutex);
    }

    pthread_exit(NULL);
}

BlockingTcpConnection::BlockingTcpConnection(void (*connectionHandler)(int, Socket*))
    : Connection(connectionHandler)
{
    connectionLaunched = false;
}

BlockingTcpConnection::~BlockingTcpConnection()
{
    serverThread.running = false;

    if (serverSocket != NULL)
        shutdown(serverSocket->getFd(), SHUT_RDWR);

    pthread_join(serverThread.pthreadAccept, NULL);

    for (std::map<int, Thread*>::iterator it = clientThreads.begin(); it != clientThreads.end(); ++it)
    {
        Thread* thread = it->second;
        Socket* socket = thread->socket;

        shutdown(socket->getFd(), SHUT_RDWR);
        pthread_join(thread->pthreadRecv, NULL);
        pthread_join(thread->pthreadSend, NULL);
        pthread_mutex_destroy(&thread->mutex);
        delete thread;
    }
}

void BlockingTcpConnection::process()
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

int BlockingTcpConnection::create()
{
	int serverFd = socket(AF_INET, SOCK_STREAM, 0);
    if (serverFd < 0)
        error("ERROR opening socket, %s", strerror(errno));

    return serverFd;
}

void BlockingTcpConnection::getNewClient()
{
    sockaddr_in address;
	socklen_t addressLen = sizeof(address);
	int clientFd = accept(serverSocket->getFd(), (sockaddr*) &address, &addressLen);
	if (clientFd < 0)
	{
        if (!serverThread.running || errno == EINTR)
            return;
        else
            error("ERROR on accept, %s", strerror(errno));
	}

	Socket* socket = new Socket(idCount, clientFd, address);
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
    if (pthread_create(&thread->pthreadRecv, NULL, clientRecvThread, thread) != 0)
        error("ERROR creating thread, %s", strerror(errno));

    connectionHandler(EVENT_CLIENT_CONNECTED, socket);

	debug("New connection from %s", inet_ntoa(address.sin_addr));
}

void BlockingTcpConnection::disconnectClientAndKillThread(Thread* thread)
{
    Socket* socket = thread->socket;

    connectionHandler(EVENT_CLIENT_DISCONNECTED, socket);

    thread->running = false;
    pthread_mutex_unlock(&thread->mutex);
    pthread_exit(NULL);
}
