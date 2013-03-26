#ifndef CONNECTION_THREAD_H
#define CONNECTION_THREAD_H

#include "connection.h"
#include <pthread.h>

struct ConnectionThread
{
        Socket*                 socket;
        pthread_t               pthreadAccept, pthreadRecv, pthreadSend;
        Connection*             connection;
        bool                    running;
        pthread_mutex_t         mutex;
};

typedef ConnectionThread Thread;

#endif
