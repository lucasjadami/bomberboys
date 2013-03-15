#ifndef SOCKET_H
#define SOCKET_H

#include <sys/socket.h>
#include <netinet/in.h>

class Socket
{
public:

                Socket(int, sockaddr_in);
    int         getFd();
    sockaddr_in getAddress();

private:

    int         fd;
    sockaddr_in address;

};

#endif
