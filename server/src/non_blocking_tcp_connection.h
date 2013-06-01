#ifndef NON_BLOCKING_TCP_CONNECTION_H
#define NON_BLOCKING_TCP_CONNECTION_H

#include "connection.h"

class NonBlockingTcpConnection : public Connection
{
public:

         NonBlockingTcpConnection(std::set<std::string>, int, void (*)(int, Socket*));
	void process();

protected:

    int  createWorldServerSocket();
    int  createServerSocket();

private:

    timeval selectTimeout;
	int 	maxFd;

	void getNewClient();
	void processClients(fd_set&, fd_set&);
	void processWorldServerSocket(fd_set&, fd_set&);
	void createFdSet(fd_set&);
	bool readFromSocket(Socket*);
	bool writeToSocket(Socket*);

};

#endif
