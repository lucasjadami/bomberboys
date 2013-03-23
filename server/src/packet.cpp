#include "packet.h"

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
