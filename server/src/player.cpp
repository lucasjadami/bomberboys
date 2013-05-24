#include "player.h"
#include "time.h"
#include <cstdlib>

Player::Player(Socket* socket)
{
    this->socket = socket;
    this->id = socket->getId();
    body = NULL;
    name = NULL;
    lastImpulse = 0;
}

Player::Player(int id)
{
    this->socket = NULL;
    this->id = id;
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
    long long now = getTimeLL(getTime(&time));
    if (now - lastImpulse > 50)
    {
        lastImpulse = now;
        body->ApplyLinearImpulse(impulse, body->GetWorldCenter());
    }
}

bool Player::isLocal()
{
    return socket != NULL;
}

int Player::getId()
{
    return id;
}
