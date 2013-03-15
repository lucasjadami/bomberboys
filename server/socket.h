#ifndef SOCKET_H
#define SOCKET_H

#include "packet.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <vector>

class Socket
{
public:

                Socket(int, sockaddr_in);
               ~Socket();
    int         getFd();
    sockaddr_in getAddress();
    char*       getInBuffer();
    void        updateInBuffer(int);

private:

    int                 fd;
    sockaddr_in         address;
    char                auxBuffer[256];
    char                inBuffer[256];
    int                 inPointer;
    std::vector<Packet*> inPackets;
};

#endif
