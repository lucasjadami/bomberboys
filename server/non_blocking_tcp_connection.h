#ifndef NON_BLOCKING_TCP_CONNECTION_H
#define NON_BLOCKING_TCP_CONNECTION_H

#include "connection.h"

class NonBlockingTcpConnection : public Connection
{
public:

	void process();

protected:

    int  create();

private:

    timeval selectTimeout;
	int 	maxFd;

	void getNewClient();
	void processClients(fd_set&);
	void createFdSet(fd_set&);

};

#endif
