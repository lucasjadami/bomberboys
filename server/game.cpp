#include "game.h"
#include "packet.h"
#include "connection.h"
#include "output.h"
#include <cstdlib>
#include <cstring>
#include <Box2D/Common/b2Math.h>
#include <Box2D/Collision/Shapes/b2PolygonShape.h>
#include <Box2D/Collision/Shapes/b2CircleShape.h>

Game::Game()
{
    world = NULL;
}

Game::~Game()
{
    if (world != NULL)
        delete world;
    for (unsigned int i = 0; i < players.size(); ++i)
        delete players[i];
}

void Game::createWorld()
{
    b2Vec2 gravity(0.0f, 0.0f);
    world = new b2World(gravity);
}

void Game::connectionHandler(int eventId, Socket* socket)
{
    if (eventId == EVENT_CLIENT_CONNECTED)
    {
        Player* newPlayer = new Player(socket);
        players.push_back(newPlayer);
    }
    else if (eventId == EVENT_CLIENT_DISCONNECTED)
    {
        std::vector<Player*>::iterator it = players.begin();
        while (it != players.end())
        {
            Player* player = *it;
            if (player->getSocket()->getId() == socket->getId())
            {
                if (player->isPlaying())
                    world->DestroyBody(player->getBody());
                delete player;
                it = players.erase(it);
            }
            else
            {
                if (player->isPlaying())
                {
                    Packet* packet = createRemovePlayerPacket(socket->getId());
                    player->getSocket()->addOutPacket(packet);
                }
                it++;
            }
        }
    }
}

void Game::update()
{
    world->Step(1.0f / 60.0f, 8, 3);

    for (unsigned int i = 0; i < players.size(); ++i)
    {
        Player* player = players[i];

        updatePlayerPackets(player);
        if (player->isPlaying())
            updatePlayerMovement(player);
    }
}

void Game::updatePlayerPackets(Player* player)
{
    Packet* packet;
    while ((packet = player->getSocket()->getInPacket()) != NULL)
    {
        if (packet->getId() == PACKET_LOGIN)
        {
            parseLoginPacket(packet, player);

            Packet* newPacket = createAddPlayerPacket(player);
            player->getSocket()->addOutPacket(newPacket);

            for (unsigned int i = 0; i < players.size(); ++i)
            {
                if (players[i] == player || !players[i]->isPlaying())
                    continue;

                newPacket = createAddPlayerPacket(player);
                players[i]->getSocket()->addOutPacket(newPacket);

                newPacket = createAddPlayerPacket(players[i]);
                player->getSocket()->addOutPacket(newPacket);
            }
        }
        else if (packet->getId() == PACKET_MOVE_ME)
        {
            parseMoveMePacket(packet, player);
        }
        delete packet;
    }
}

void Game::updatePlayerMovement(Player* player)
{
    if (player->isLastPositionDifferent())
    {
        for (unsigned int i = 0; i < players.size(); ++i)
        {
            if (!players[i]->isPlaying())
                continue;

            Packet* newPacket = createMovePlayerPacket(player);
            players[i]->getSocket()->addOutPacket(newPacket);
        }
    }

    player->saveLastPosition();
}

void Game::createPlayerBody(Player* player)
{
    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.position.Set(MAP_WIDTH / 2, MAP_HEIGHT / 2);

    b2Body* body = world->CreateBody(&bodyDef);

    b2CircleShape dynamicCircle;
    dynamicCircle.m_radius = 10.0f;

    b2FixtureDef fixtureDef;
    fixtureDef.shape = &dynamicCircle;
    fixtureDef.density = 0.01f;
    fixtureDef.friction = 0.3f;

    body->CreateFixture(&fixtureDef);
    body->SetLinearDamping(0.1f);

    player->setBody(body);
}

void Game::parseLoginPacket(Packet* packet, Player* player)
{
    char* name = new char[NAME_SIZE];
    memcpy(name, packet->getData(), sizeof(char) * PACKET_LOGIN_SIZE);
    name[NAME_SIZE - 1] = '\0';
    player->setName(name);

    createPlayerBody(player);
    player->saveLastPosition();
}

void Game::parseMoveMePacket(Packet* packet, Player* player)
{
    int direction = packet->getData()[0];

    b2Vec2 impulse;
    impulse.SetZero();

    switch (direction)
    {
        case 0:
            impulse.y = 10.0f; break;
        case 2:
            impulse.x = 10.0f; break;
        case 4:
            impulse.y = -10.0f; break;
        case 6:
            impulse.x = -10.0f; break;
    }

    player->getBody()->ApplyLinearImpulse(impulse, player->getBody()->GetWorldCenter());
}

Packet* Game::createAddPlayerPacket(Player* player)
{
    char* data = new char[PACKET_ADD_PLAYER_SIZE];
    memset(data, 0, sizeof(char) * PACKET_ADD_PLAYER_SIZE);
    putBytes(data, player->getSocket()->getId(), 2);
    putBytes(data + 2, player->getBody()->GetPosition().x, 2);
    putBytes(data + 4, player->getBody()->GetPosition().y, 2);
    memcpy(data + 6, player->getName(), sizeof(char) * NAME_SIZE);
    Packet* packet = new Packet(PACKET_ADD_PLAYER, data);
    return packet;
}

Packet* Game::createRemovePlayerPacket(int id)
{
    char* data = new char[PACKET_REMOVE_PLAYER_SIZE];
    memset(data, 0, sizeof(char) * PACKET_REMOVE_PLAYER_SIZE);
    putBytes(data, id, 2);
    return new Packet(PACKET_REMOVE_PLAYER, data);
}

Packet* Game::createMovePlayerPacket(Player* player)
{
    char* data = new char[PACKET_MOVE_PLAYER_SIZE];
    memset(data, 0, sizeof(char) * PACKET_MOVE_PLAYER_SIZE);
    putBytes(data, player->getSocket()->getId(), 2);
    putBytes(data + 2, (int) player->getBody()->GetPosition().x, 2);
    putBytes(data + 4, (int) player->getBody()->GetPosition().y, 2);
    return new Packet(PACKET_MOVE_PLAYER, data);
}

void Game::putBytes(char* data, int value, int bytes)
{
    int mask = 0xFF;

    for (int i = 0; i < bytes; ++i)
    {
        data[bytes - i - 1] = (value & mask) >> (8 * i);
        mask = mask << 8;
    }
}
