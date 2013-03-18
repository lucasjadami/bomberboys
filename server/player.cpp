#include "player.h"
#include <cstdlib>

Player::Player(Socket* socket)
{
    this->socket = socket;
    body = NULL;
    name = NULL;
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
