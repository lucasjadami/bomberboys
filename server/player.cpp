#include "player.h"
#include "output.h"
#include <cstdlib>
#include <time.h>

Player::Player(Socket* socket)
{
    this->socket = socket;
    body = NULL;
    name = NULL;
    lastImpulse = 0;
}

Player::~Player()
{
    if (name != NULL)
        delete[] name;
}

char* Player::getName()
{
    return name;
}

void Player::setName(char* name)
{
    this->name = name;
}

Socket* Player::getSocket()
{
    return socket;
}

bool Player::isPlaying()
{
    return name != NULL;
}

b2Body* Player::getBody()
{
    return body;
}

void Player::setBody(b2Body* body)
{
    this->body = body;
}

void Player::saveLastPosition()
{
    lastX = (int) body->GetPosition().x;
    lastY = (int) body->GetPosition().y;
}

bool Player::isLastPositionDifferent()
{
    return lastX != (int) body->GetPosition().x || lastY != (int) body->GetPosition().y;
}

void Player::applyImpulse(b2Vec2& impulse)
{
    timespec time;
    if (clock_gettime(CLOCK_REALTIME, &time) == -1)
        error("ERROR on gettime");

    long long now = time.tv_sec * 1000LL + time.tv_nsec / 1000000;
    if (now - lastImpulse > 50)
    {
        lastImpulse = now;
        body->ApplyLinearImpulse(impulse, body->GetWorldCenter());
    }
}
