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
    lastAck = getTimeLL(getTime(&time));
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
    long long now = getTimeLL(getTime(&time));
    if (now - lastImpulse > 50)
    {
        lastImpulse = now;
        body->ApplyLinearImpulse(impulse, body->GetWorldCenter());
    }
}

bool Player::isIdle()
{
    timespec time;
    long long now = getTimeLL(getTime(&time));

    return now - lastAck > 10000;
}

void Player::updateLastAck()
{
    timespec time;
    lastAck = getTimeLL(getTime(&time));
}
