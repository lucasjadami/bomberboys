#ifndef BLOCKING_UDP_CONNECTION_H
#define BLOCKING_UDP_CONNECTION_H

#include "connection.h"
#include "connection_thread.h"

class BlockingUdpConnection : public Connection
{
public:

         BlockingUdpConnection(void (*)(int, Socket*));
        ~BlockingUdpConnection();
	void process();
	void disconnectClientAndStopThread(Thread*);
	void readFromClient();

protected:

    int  create();

private:

    bool                            connectionLaunched;

    // IP addresses are identified by numbers.
    short                           addressIdentifierCount;
    std::map<unsigned long, short>  addressIdentifiers;

    // The descriptors on UDP don't exist. The fd used is based on IP + port.
    // This map maps fds into ids.
    std::map<int, int>              clientDescriptors;

    std::map<int, Thread*>          clientThreads;
    Thread                          serverThread;

};

#endif
