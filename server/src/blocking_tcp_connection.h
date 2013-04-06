#ifndef BLOCKING_TCP_CONNECTION_H
#define BLOCKING_TCP_CONNECTION_H

#include "connection.h"
#include "connection_thread.h"
#include <map>
#include <pthread.h>

class BlockingTcpConnection : public Connection
{
public:

          BlockingTcpConnection(void (*)(int, Socket*));
         ~BlockingTcpConnection();
	void  process();
	void  getNewClient();
	void  disconnectClientAndKillThread(Thread*);
	void  destroyClient(int);

protected:

    int   create();

private:

    pthread_mutex_t         mapsMutex;
    std::map<int, Thread*>  clientThreads;
    Thread                  serverThread;
    bool                    connectionLaunched;

};

#endif
