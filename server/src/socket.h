#ifndef SOCKET_H
#define SOCKET_H

#include "packet.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <vector>

#define BUFFER_SIZE 4096

class Socket
{
public:

                Socket(int, int, sockaddr_in);
                Socket(int, int, addrinfo*);
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
    bool        isDisconnectForced();
    void        setDisconnectForced(bool);

private:

    int                     id;
    int                     fd;
    sockaddr_in             address;
    addrinfo*               addressInfo;
    char                    auxBuffer[BUFFER_SIZE];
    char                    inBuffer[BUFFER_SIZE];
    char                    outBuffer[BUFFER_SIZE];
    int                     inPointer;
    int                     outPointer;
    std::vector<Packet*>    inPackets;
    std::vector<Packet*>    outPackets;
    int                     packetUId;
    bool                    disconnectForced;

};

#endif
