#include "packet.h"
#include <climits>
#include <cstring>

int Packet::getSize(int id)
{
    int size = INT_MAX / 2;
    switch (id)
    {
        case PACKET_LOGIN:
            size = PACKET_LOGIN_SIZE; break;
        case PACKET_ADD_PLAYER:
            size = PACKET_ADD_PLAYER_SIZE; break;
        case PACKET_REMOVE_PLAYER:
            size = PACKET_REMOVE_PLAYER_SIZE; break;
        case PACKET_MOVE_ME:
            size = PACKET_MOVE_ME_SIZE; break;
        case PACKET_MOVE_PLAYER:
            size = PACKET_MOVE_PLAYER_SIZE; break;
        case PACKET_PLANT_BOMB:
            size = PACKET_PLANT_BOMB_SIZE; break;
        case PACKET_ADD_BOMB:
            size = PACKET_ADD_BOMB_SIZE; break;
        case PACKET_EXPLODE_BOMB:
            size = PACKET_EXPLODE_BOMB_SIZE; break;
        case PACKET_FALL_PLAYER:
            size = PACKET_FALL_PLAYER_SIZE; break;
        case PACKET_SHUTDOWN:
            size = PACKET_SHUTDOWN_SIZE; break;
        case PACKET_LOGIN_EX:
            size = PACKET_LOGIN_EX_SIZE; break;
        case PACKET_ADD_PLAYER_EX:
            size = PACKET_ADD_PLAYER_EX_SIZE; break;
        case PACKET_MOVE_ME_EX:
            size = PACKET_MOVE_ME_EX_SIZE; break;
        case PACKET_PLANT_BOMB_EX:
            size = PACKET_PLANT_BOMB_EX_SIZE; break;
    }
    return size;
}

void Packet::putIntBytes(char* data, ULL value, int bytes)
{
    int mask = 0;

    for (int i = 0; i < bytes; ++i)
    {
        data[bytes - i - 1] = (value & (255ULL << mask)) >> mask;
        mask += 8;
    }
}

int Packet::getInt(char* data)
{
    int value = 0;
    int mask = 0;

    for (int i = 3; i > -1; --i)
    {
        int buffer = (unsigned char) data[i];
        value |= buffer << mask;
        mask += 8;
    }

    return value;
}

short Packet::getShort(char* data)
{
    short value = 0;
    int mask = 0;

    for (int i = 1; i > -1; --i)
    {
        short buffer = (unsigned char) data[i];
        value |= buffer << mask;
        mask += 8;
    }

    return value;
}

ULL Packet::getULongLong(char* data)
{
    ULL value = 0;
    int mask = 0;

    for (int i = 7; i > -1; --i)
    {
        ULL buffer = (unsigned char) data[i];
        value |= buffer << mask;
        mask += 8;
    }

    return value;
}

double Packet::getDouble(char* data)
{
    LL numerator = 0;
    int mask = 0;

    for (int i = 7; i > -1; --i)
    {
        LL value = (unsigned char) data[i];
        numerator |= value << mask;
        mask += 8;
    }

    LL denominator = 0;
    mask = 0;
    data += 8;

    for (int i = 7; i > -1; --i)
    {
        LL value = (unsigned char) data[i];
        denominator |= value << mask;
        mask += 8;
    }

    return (double) numerator / denominator;
}

Packet::Packet(int id, char* data)
{
    this->id = id;
    this->data = data;
}

Packet::~Packet()
{
    delete[] data;
}

int Packet::getId()
{
    return id;
}

char* Packet::getData()
{
    return data;
}

Packet* Packet::clone(int newId)
{
    int newSize = getSize(newId);
    int size = getSize(this->id);
    char* data = new char[newSize];
    // The newId must incur a newSize bigger than this id.
    memcpy(data + newSize - size, this->getData(), size);
    Packet* packet = new Packet(newId, data);
    return packet;
}
