#include "player.h"
#include <cstdlib>

Player::Player(Socket* socket, int x, int y)
{
    this->socket = socket;
    this->x = x;
    this->y = y;
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

int Player::getX()
{
    return x;
}

int Player::getY()
{
    return y;
}
