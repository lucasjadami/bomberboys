#ifndef NON_BLOCKING_TCP_CONNECTION_H
#define NON_BLOCKING_TCP_CONNECTION_H

#include "connection.h"

class NonBlockingTcpConnection : public Connection
{
public:

	void create();	
	void process();

private:

	fd_set 	readFdSet;
	timeval selectTimeout;

};

#endif
