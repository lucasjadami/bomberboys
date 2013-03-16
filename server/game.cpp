#include "game.h"
#include "packet.h"
#include "connection.h"
#include <cstdlib>
#include <cstring>

Game::Game()
{

}

Game::~Game()
{
    for (unsigned int i = 0; i < players.size(); ++i)
        delete players[i];
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
    data[0] = PACKET_ADD_PLAYER;
    memcpy(data + 1, player->getName(), sizeof(char) * NAME_SIZE);
    Packet* packet = new Packet(PACKET_ADD_PLAYER, data);
    return packet;
}

Packet* Game::createRemovePlayerPacket(int id)
{
    char* data = new char[PACKET_REMOVE_PLAYER_SIZE];
    memset(data, 0, sizeof(char) * PACKET_REMOVE_PLAYER_SIZE);
    data[0] = id & 0xFF;
    data[1] = id & 0xFF00;
    return new Packet(PACKET_REMOVE_PLAYER, data);
}
