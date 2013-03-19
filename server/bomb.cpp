#include "bomb.h"
#include "output.h"
#include <time.h>

Bomb::Bomb(int id)
{
    this->id = id;

    timespec time;
    if (clock_gettime(CLOCK_REALTIME, &time) == -1)
        error("ERROR on gettime");

    plantedTime = time.tv_sec * 1000LL + time.tv_nsec / 1000000;
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
    if (clock_gettime(CLOCK_REALTIME, &time) == -1)
        error("ERROR on gettime");

    long long now = time.tv_sec * 1000LL + time.tv_nsec / 1000000;
    return now - plantedTime > BOMB_TIME;
}

int Bomb::getId()
{
    return id;
}
