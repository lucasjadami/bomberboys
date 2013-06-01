#include "socket.h"
#include "output.h"
#include <cstring>

Socket::Socket(int id, int fd, sockaddr_in address)
{
    this->id = id;
    this->fd = fd;
    this->address = address;
    addressInfo = NULL;
    inPointer = 0;
    outPointer = 0;
    memset(inBuffer, 0, sizeof(inBuffer));
    memset(outBuffer, 0, sizeof(outBuffer));
    packetUId = 0;
    disconnectForced = false;
}

Socket::Socket(int id, int fd, addrinfo* addressInfo)
{
    // Unfortunatelly, this C++ version does not have constructor-constructor calls.
    this->id = id;
    this->fd = fd;
    this->address = *((sockaddr_in*) addressInfo->ai_addr);
    this->addressInfo = addressInfo;
    inPointer = 0;
    outPointer = 0;
    memset(inBuffer, 0, sizeof(inBuffer));
    memset(outBuffer, 0, sizeof(outBuffer));
    packetUId = 0;
    disconnectForced = false;
}

Socket::~Socket()
{
    if (addressInfo != NULL)
        freeaddrinfo(addressInfo);

    for (unsigned int i = 0; i < inPackets.size(); ++i)
        delete inPackets[i];
    for (unsigned int i = 0; i < outPackets.size(); ++i)
        delete outPackets[i];
}

sockaddr_in Socket::getAddress()
{
    return address;
}

int Socket::getId()
{
    return id;
}

int Socket::getFd()
{
    return fd;
}

char* Socket::getInBuffer()
{
    return inBuffer + inPointer;
}

int Socket::getInBufferSize()
{
    return sizeof(char) * (BUFFER_SIZE - inPointer);
}

char* Socket::getOutBuffer()
{
    return outBuffer;
}

int Socket::getOutBufferSize()
{
    return sizeof(char) * outPointer;
}

bool Socket::updateInBuffer(int& bytesRead)
{
    inPointer += bytesRead;

    bytesRead = 0;

    if (inPointer > 1)
    {
        int packetId = inBuffer[0];
        int size = Packet::getSize(packetId);

        if (inPointer > size)
        {
            char* data = new char[size];
            memcpy(data, inBuffer + 1, sizeof(char) * size);

            inPackets.push_back(new Packet(packetId, data));

            memcpy(auxBuffer, inBuffer + size + 1, sizeof(char) * (BUFFER_SIZE - (size + 1)));
            memcpy(inBuffer, auxBuffer, sizeof(char) * BUFFER_SIZE);

            inPointer -= size + 1;

            return true;
        }
    }
    return false;
}

bool Socket::updateOutBuffer(int& bytesWritten)
{
    bool retValue = false;

    if (bytesWritten > 0)
    {
        outPointer -= bytesWritten;

        memcpy(auxBuffer, outBuffer + outPointer, sizeof(char) * (BUFFER_SIZE -  outPointer));
        memcpy(outBuffer, auxBuffer, sizeof(auxBuffer));
    }

    bytesWritten = 0;

    Packet* packet = outPackets.empty() ? NULL : outPackets[0];
    if (packet != NULL)
    {
        int size = Packet::getSize(packet->getId());
        if (outPointer + size + 1 < BUFFER_SIZE)
        {
            outBuffer[outPointer] = packet->getId();
            memcpy(outBuffer + outPointer + 1, packet->getData(), sizeof(char) * size);

            outPointer += 1 + size;

            delete packet;
            outPackets.erase(outPackets.begin());

            retValue = true;
        }
    }

    return retValue;
}

void Socket::addOutPacket(Packet* packet)
{
    outPackets.push_back(packet);
}

Packet* Socket::getInPacket()
{
    Packet* packet = NULL;

    if (!inPackets.empty())
    {
        packet = inPackets[0];
        inPackets.erase(inPackets.begin());
    }

    return packet;
}

void Socket::appendInBuffer(char* buffer, int size)
{
    if (size + inPointer <= BUFFER_SIZE)
        memcpy(inBuffer, buffer, sizeof(char) * size);
}

bool Socket::isDisconnectForced()
{
    return disconnectForced;
}

void Socket::setDisconnectForced(bool disconnectForced)
{
    this->disconnectForced = disconnectForced;
}
