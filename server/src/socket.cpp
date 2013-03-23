#include "socket.h"
#include <cstring>
#include <climits>

Socket::Socket(int id, int fd, sockaddr_in address)
{
    this->id = id;
    this->fd = fd;
    this->address = address;
    inPointer = 0;
    outPointer = 0;
    memset(inBuffer, 0, sizeof(inBuffer));
    memset(outBuffer, 0, sizeof(outBuffer));
}

Socket::~Socket()
{
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

void Socket::updateInBuffer(int bytesRead)
{
    inPointer += bytesRead;

    if (inPointer > 0)
    {
        int packetId = inBuffer[0];
        int size = INT_MAX / 2;
        switch (packetId)
        {
            case PACKET_LOGIN:
                size = PACKET_LOGIN_SIZE; break;
            case PACKET_MOVE_ME:
                size = PACKET_MOVE_ME_SIZE; break;
            case PACKET_PLANT_BOMB:
                size = PACKET_PLANT_BOMB_SIZE; break;
        }

        if (inPointer > size)
        {
            char* data = new char[size];
            memcpy(data, inBuffer + 1, sizeof(char) * size);
            inPackets.push_back(new Packet(packetId, data));

            memcpy(auxBuffer, inBuffer + size + 1, sizeof(char) * (BUFFER_SIZE - (size + 1)));
            memcpy(inBuffer, auxBuffer, sizeof(char) * BUFFER_SIZE);

            inPointer -= size + 1;

            updateInBuffer(0);
        }
    }
}

void Socket::updateOutBuffer(int bytesWritten)
{
    if (bytesWritten > 0)
    {
        outPointer -= bytesWritten;

        memcpy(auxBuffer, outBuffer + outPointer, sizeof(char) * (BUFFER_SIZE -  outPointer));
        memcpy(outBuffer, auxBuffer, sizeof(auxBuffer));
    }

    Packet* packet = outPackets.empty() ? NULL : outPackets[0];
    if (packet != NULL)
    {
        int size = INT_MAX / 2;
        switch (packet->getId())
        {
            case PACKET_ADD_PLAYER:
                size = PACKET_ADD_PLAYER_SIZE; break;
            case PACKET_REMOVE_PLAYER:
                size = PACKET_REMOVE_PLAYER_SIZE; break;
            case PACKET_MOVE_PLAYER:
                size = PACKET_MOVE_PLAYER_SIZE; break;
            case PACKET_ADD_BOMB:
                size = PACKET_ADD_BOMB_SIZE; break;
            case PACKET_EXPLODE_BOMB:
                size = PACKET_EXPLODE_BOMB_SIZE; break;
            case PACKET_FALL_PLAYER:
                size = PACKET_FALL_PLAYER_SIZE; break;
        }

        if (outPointer + size + 1 <= BUFFER_SIZE)
        {
            outBuffer[outPointer] = packet->getId();
            memcpy(outBuffer + outPointer + 1, packet->getData(), sizeof(char) * size);

            outPointer += 1 + size;

            delete packet;
            outPackets.erase(outPackets.begin());
        }
    }
}

void Socket::addOutPacket(Packet* packet)
{
    outPackets.push_back(packet);
}

Packet* Socket::getInPacket()
{
    if (inPackets.empty())
        return NULL;

    Packet* packet = inPackets[0];
    inPackets.erase(inPackets.begin());
    return packet;
}
