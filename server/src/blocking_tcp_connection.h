#ifndef BLOCKING_TCP_CONNECTION_H
#define BLOCKING_TCP_CONNECTION_H

#include "connection.h"
#include "connection_thread.h"
#include <map>

class BlockingTcpConnection : public Connection
{
public:

          BlockingTcpConnection(void (*)(int, Socket*));
         ~BlockingTcpConnection();
	void  process();
	void  getNewClient();
	void  disconnectClientAndKillThread(Thread*);

protected:

    int   create();

private:

    std::map<int, Thread*>  clientThreads;
    Thread                  serverThread;
    bool                    connectionLaunched;

};

#endif
