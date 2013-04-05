#include "player.h"
#include "time.h"
#include <cstdlib>

Player::Player(Socket* socket)
{
    this->socket = socket;
    body = NULL;
    name = NULL;
    lastImpulse = 0;

    timespec time;
    getTime(&time);

    lastAck = time.tv_sec * 1000LL + time.tv_nsec / 1000000;
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
    getTime(&time);

    long long now = time.tv_sec * 1000LL + time.tv_nsec / 1000000;
    if (now - lastImpulse > 50)
    {
        lastImpulse = now;
        body->ApplyLinearImpulse(impulse, body->GetWorldCenter());
    }
}

bool Player::isIdle()
{
    timespec time;
    getTime(&time);
    long long now = time.tv_sec * 1000LL + time.tv_nsec / 1000000;

    return now - lastAck > 10000;
}

void Player::updateLastAck()
{
    timespec time;
    getTime(&time);
    lastAck = time.tv_sec * 1000LL + time.tv_nsec / 1000000;
}
