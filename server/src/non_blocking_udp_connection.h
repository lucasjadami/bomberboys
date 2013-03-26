#ifndef NON_BLOCKING_UDP_CONNECTION_H
#define NON_BLOCKING_UDP_CONNECTION_H

#include "connection.h"

class NonBlockingUdpConnection : public Connection
{
public:

         NonBlockingUdpConnection(void (*)(int, Socket*));
	void process();

protected:

    int  create();

private:

    timeval                         selectTimeout;

    // IP addresses are identified by numbers.
    short                           addressIdentifierCount;
    std::map<unsigned long, short>  addressIdentifiers;

    // The descriptors on UDP don't exist. The fd used is based on IP + port.
    // This map maps fds into ids.
    std::map<int, int>              clientDescriptors;

	void readFromClient();
	void writeToClients();
	void createFdSet(fd_set&);

};

#endif
