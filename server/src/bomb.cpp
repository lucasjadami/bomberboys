#include "bomb.h"
#include "time.h"

Bomb::Bomb(int id)
{
    this->id = id;

    timespec time;
    plantedTime = getTimeLL(getTime(&time));
}

b2Body* Bomb::getBody()
{
    return body;
}

void Bomb::setBody(b2Body* body)
{
    this->body = body;
}

bool Bomb::explode()
{
    timespec time;
    long long now = getTimeLL(getTime(&time));
    return now - plantedTime > BOMB_TIME;
}

int Bomb::getId()
{
    return id;
}
