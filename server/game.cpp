#include "game.h"
#include "packet.h"
#include "connection.h"
#include <cstdlib>
#include <cstring>
#include <Box2D/Common/b2Math.h>

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

    b2AABB bounds;
    bounds.lowerBound = b2Vec2(0.0f, 0.0f);
    bounds.upperBound = b2Vec2(MAP_WIDTH, MAP_HEIGHT);

    world = new b2World(bounds, gravity, true);
}

void Game::connectionHandler(int eventId, Socket* socket)
{
    if (eventId == EVENT_CLIENT_CONNECTED)
    {
        Player* newPlayer = new Player(socket, MAP_WIDTH / 2, MAP_HEIGHT / 2);
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

void Game::updateGamePackets()
{
    for (unsigned int i = 0; i < players.size(); ++i)
    {
        Player* player = players[i];
        Packet* packet;
        if ((packet = player->getSocket()->getInPacket()) != NULL)
        {
            if (packet->getId() == PACKET_LOGIN)
            {
                parseLoginPacket(packet, player);

                Packet* newPacket = createAddPlayerPacket(player);
                player->getSocket()->addOutPacket(newPacket);

                for (unsigned int j = 0; j < players.size(); ++j)
                {
                    if (j == i || !players[j]->isPlaying())
                        continue;

                    newPacket = createAddPlayerPacket(player);
                    players[j]->getSocket()->addOutPacket(newPacket);

                    newPacket = createAddPlayerPacket(players[j]);
                    player->getSocket()->addOutPacket(newPacket);
                }
            }
            delete packet;
        }
    }
}

void Game::parseLoginPacket(Packet* packet, Player* player)
{
    char* name = new char[NAME_SIZE];
    memcpy(name, packet->getData(), sizeof(char) * PACKET_LOGIN_SIZE);
    name[NAME_SIZE - 1] = '\0';
    player->setName(name);
}

Packet* Game::createAddPlayerPacket(Player* player)
{
    char* data = new char[PACKET_ADD_PLAYER_SIZE];
    memset(data, 0, sizeof(char) * PACKET_ADD_PLAYER_SIZE);
    putBytes(data, player->getX(), 2);
    putBytes(data + 2, player->getY(), 2);
    memcpy(data + 5, player->getName(), sizeof(char) * NAME_SIZE);
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

void Game::putBytes(char* data, int value, int bytes)
{
    int mask = 0xFF;

    for (int i = 0; i < bytes; ++i)
    {
        data[i] = value & mask;
        mask = mask << 8;
    }
}
