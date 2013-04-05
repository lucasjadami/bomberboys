#include "socket.h"
#include "output.h"
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
    packetUId = 0;
    disconnectForced = false;

#ifdef BLOCKING_MODE
    pthread_mutex_init(&inMutex, NULL);
    pthread_mutex_init(&outMutex, NULL);
#endif
}

Socket::~Socket()
{
    for (unsigned int i = 0; i < inPackets.size(); ++i)
        delete inPackets[i];
    for (unsigned int i = 0; i < outPackets.size(); ++i)
        delete outPackets[i];

#ifdef BLOCKING_MODE
    pthread_mutex_destroy(&inMutex);
    pthread_mutex_destroy(&outMutex);
#endif
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

    if (inPointer > PACKET_UID_SIZE)
    {
        int packetId = inBuffer[PACKET_UID_SIZE];
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

        if (inPointer > PACKET_UID_SIZE + size)
        {
            char* uid = new char[PACKET_UID_SIZE];
            char* data = new char[size];
            memcpy(uid, inBuffer, sizeof(char) * PACKET_UID_SIZE);
            memcpy(data, inBuffer + PACKET_UID_SIZE + 1, sizeof(char) * size);

#ifdef BLOCKING_MODE
            pthread_mutex_lock(&inMutex);
#endif
            inPackets.push_back(new Packet(Packet::getInt(uid), packetId, data));
#ifdef BLOCKING_MODE
            pthread_mutex_unlock(&inMutex);
#endif

            memcpy(auxBuffer, inBuffer + size + PACKET_UID_SIZE + 1, sizeof(char) * (BUFFER_SIZE - (size + PACKET_UID_SIZE + 1)));
            memcpy(inBuffer, auxBuffer, sizeof(char) * BUFFER_SIZE);

            inPointer -= size + PACKET_UID_SIZE + 1;

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

#ifdef BLOCKING_MODE
    pthread_mutex_lock(&outMutex);
#endif

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

        if (outPointer + size + PACKET_UID_SIZE + 1 <= BUFFER_SIZE)
        {
            Packet::putBytes(outBuffer + outPointer, packet->getUId(), 4);
            outBuffer[outPointer + PACKET_UID_SIZE] = packet->getId();
            memcpy(outBuffer + outPointer + PACKET_UID_SIZE + 1, packet->getData(), sizeof(char) * size);

            outPointer += 1 + PACKET_UID_SIZE + size;

            delete packet;
            outPackets.erase(outPackets.begin());

            retValue = true;
        }
    }

#ifdef BLOCKING_MODE
    pthread_mutex_unlock(&outMutex);
#endif

    return retValue;
}

void Socket::addOutPacket(Packet* packet)
{
#ifdef BLOCKING_MODE
    pthread_mutex_lock(&outMutex);
#endif
    if (packet->getUId() == 0)
        packet->setUId(packetUId++);
    outPackets.push_back(packet);
#ifdef BLOCKING_MODE
    pthread_mutex_unlock(&outMutex);
#endif
}

Packet* Socket::getInPacket()
{
    Packet* packet = NULL;

#ifdef BLOCKING_MODE
    pthread_mutex_lock(&inMutex);
#endif

    if (!inPackets.empty())
    {
        packet = inPackets[0];
        inPackets.erase(inPackets.begin());
    }

#ifdef BLOCKING_MODE
    pthread_mutex_unlock(&inMutex);
#endif

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
