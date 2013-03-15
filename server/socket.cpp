#include "socket.h"
#include <cstring>

Socket::Socket(int fd, sockaddr_in address)
{
    this->fd = fd;
    this->address = address;
    inPointer = 0;
    memset(inBuffer, 0, sizeof(inBuffer));
}

Socket::~Socket()
{
    for (unsigned int i = 0; i < inPackets.size(); ++i)
        delete inPackets[i];
}

sockaddr_in Socket::getAddress()
{
    return address;
}

int Socket::getFd()
{
    return fd;
}

char* Socket::getInBuffer()
{
    return inBuffer + inPointer;
}

void Socket::updateInBuffer(int bytesRead)
{
    inPointer += bytesRead;

    if (inPointer > 0)
    {
        int packetId = inBuffer[0];
        if (packetId == PACKET_LOGIN)
        {
            if (inPointer > PACKET_LOGIN_SIZE)
            {
                char* data = new char[PACKET_LOGIN_SIZE];
                memcpy(data, inBuffer + 1, sizeof(char) * PACKET_LOGIN_SIZE);
                inPackets.push_back(new Packet(PACKET_LOGIN, data));

                memcpy(auxBuffer, inBuffer + inPointer, sizeof(inBuffer + inPointer));
                memcpy(inBuffer, auxBuffer, sizeof(auxBuffer));

                inPointer -= PACKET_LOGIN_SIZE + 1;
            }
        }
    }
}
