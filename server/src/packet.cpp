#include "packet.h"

void Packet::putBytes(char* data, int value, int bytes)
{
    int mask = 0xFF;

    for (int i = 0; i < bytes; ++i)
    {
        data[bytes - i - 1] = (value & mask) >> (8 * i);
        mask = mask << 8;
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

double Packet::getDouble(char* data)
{
    long long numerator = 0;
    int mask = 0;

    for (int i = 0; i < 8; ++i)
    {
        long long value = (unsigned char) data[i];
        numerator |= value << mask;
        mask += 8;
    }

    long long denominator = 0;
    mask = 0;
    data += 8;

    for (int i = 0; i < 8; ++i)
    {
        long long value = (unsigned char) data[i];
        denominator |= value << mask;
        mask += 8;
    }

    return (double) numerator / denominator;
}

Packet::Packet(int id, char* data)
{
    uId = 0;
    this->id = id;
    this->data = data;
}

Packet::Packet(int uId, int id, char* data)
{
    this->uId = uId;
    this->id = id;
    this->data = data;
}

Packet::~Packet()
{
    delete[] data;
}

int Packet::getUId()
{
    return uId;
}

void Packet::setUId(int uId)
{
    this->uId = uId;
}

int Packet::getId()
{
    return id;
}

char* Packet::getData()
{
    return data;
}
