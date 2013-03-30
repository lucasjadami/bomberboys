#ifndef SOCKET_H
#define SOCKET_H

#include "packet.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <vector>

#ifdef BLOCKING_TCP_CONNECTION
#include <pthread.h>
#endif

#define BUFFER_SIZE 256

class Socket
{
public:

                Socket(int, int, sockaddr_in);
               ~Socket();
    int         getId();
    int         getFd();
    sockaddr_in getAddress();
    char*       getInBuffer();
    int         getInBufferSize();
    char*       getOutBuffer();
    int         getOutBufferSize();
    bool        updateInBuffer(int&);
    bool        updateOutBuffer(int&);
    void        addOutPacket(Packet*);
    Packet*     getInPacket();
    void        appendInBuffer(char*, int);

private:

    int                     id;
    int                     fd;
    sockaddr_in             address;
    char                    auxBuffer[BUFFER_SIZE];
    char                    inBuffer[BUFFER_SIZE];
    char                    outBuffer[BUFFER_SIZE];
    int                     inPointer;
    int                     outPointer;
    std::vector<Packet*>    inPackets;
    std::vector<Packet*>    outPackets;

#ifdef BLOCKING_TCP_CONNECTION
    pthread_mutex_t         inMutex;
    pthread_mutex_t         outMutex;
#endif

};

#endif
