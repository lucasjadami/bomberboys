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
    short                           addressIdentifierCount;
    std::map<unsigned long, short>  addressIdentifiers;
    // The descriptors on UDP don't exist. The fd used is based on IP + port.
    std::map<int, int>              clientDescriptors;

	void readClient();
	void writeClients();
	void createFdSet(fd_set&);

};

#endif
