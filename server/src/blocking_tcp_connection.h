#ifndef BLOCKING_TCP_CONNECTION_H
#define BLOCKING_TCP_CONNECTION_H

#include "connection.h"
#include <pthread.h>
#include <map>

class BlockingTcpConnection;

struct Thread
{
        Socket*                 socket;
        pthread_t               pthreadAccept, pthreadRecv, pthreadSend;
        BlockingTcpConnection*  connection;
        bool                    running;
        pthread_mutex_t         mutex;
};

class BlockingTcpConnection : public Connection
{
public:

          BlockingTcpConnection(void (*)(int, Socket*));
         ~BlockingTcpConnection();
	void  process();
	void  getNewClient();
	void  disconnectClientAndKillThreads(Thread*, bool);

protected:

    int   create();

private:

    std::map<int, Thread*>  clientThreads;
    Thread                  serverThread;
    bool                    connectionLaunched;

};

#endif
