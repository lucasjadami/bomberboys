#include "player.h"
#include "time.h"
#include <cstdlib>

Player::Player(Socket* socket)
{
    this->socket = socket;
    this->id = socket->getId();
    sId = 0;
    body = NULL;
    name = new char[NAME_SIZE];
    memset(name, 0, sizeof(char) * NAME_SIZE);
    lastImpulse = 0;
    lastAlive = 0;
    lastX = -1;
    lastY = -1;
}

Player::Player(int id)
{
    this->socket = NULL;
    this->id = id;
    sId = 0;
    body = NULL;
    name = new char[NAME_SIZE];
    memset(name, 0, sizeof(char) * NAME_SIZE);
    lastImpulse = 0;
    lastAlive = 0;
    lastX = -1;
    lastY = -1;
}

Player::~Player()
{
    delete[] name;
}

char* Player::getName()
{
    return name;
}

void Player::setName(char* name)
{
    strcpy(this->name, name);
}

Socket* Player::getSocket()
{
    return socket;
}

void Player::setSocket(Socket* socket)
{
    this->socket = socket;
}

bool Player::isPlaying()
{
    return sId != 0;
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
    LL now = getTimeLL(getTime(&time));
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

ULL Player::getSId()
{
    return sId;
}

void Player::setSId(ULL sId)
{
    this->sId = sId;
}

LL Player::getLastAlive()
{
    return lastAlive;
}

void Player::setLastAlive()
{
    timespec time;
    lastAlive = getTimeLL(getTime(&time));
}

bool Player::isIdle()
{
    timespec time;
    return getTimeLL(getTime(&time)) - lastAlive > RECONNECT_INTERVAL;
}
