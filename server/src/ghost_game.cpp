#include "ghost_game.h"
#include "connection.h"

GhostGame::~GhostGame()
{
    if (server != NULL)
        delete server;
}

void GhostGame::connectionHandler(int eventId, Socket* socket)
{
    if (eventId == EVENT_SERVER_CONNECTED)
    {
        server = new Server(socket);
    }
    else if (eventId == EVENT_CLIENT_CONNECTED)
    {
        Player* newPlayer = new Player(socket);
        Packet* newPacket = createAddPlayerPacket(newPlayer);
        server->getSocket()->addOutPacket(newPacket);
    }
    else if (eventId == EVENT_CLIENT_DISCONNECTED)
    {
        std::map<int, Player*>::iterator it = players.begin();
        while (it != players.end())
        {
            Player* player = it->second;
            if (player->getSocket()->getId() == socket->getId())
            {
                Packet* newPacket = createRemovePlayerPacket(player->getSocket()->getId());
                server->getSocket()->addOutPacket(newPacket);
            }
        }
    }
}

void GhostGame::update(float time, float velocityIteration, float positionIterations)
{
    updateServerSocket();
}

void GhostGame::updateServerSocket()
{
    Packet* packet;
    while ((packet = server->getSocket()->getInPacket()) != NULL)
    {
        if (packet->getId() == PACKET_ADD_PLAYER)
            parseAddPlayerPacket(packet);
        else if (packet->getId() == PACKET_REMOVE_PLAYER)
            parseRemovePlayerPacket(packet);
        else if (packet->getId() == PACKET_MOVE_PLAYER)
            parseMovePlayerPacket(packet);
        else if (packet->getId() == PACKET_ADD_BOMB)
            parseAddBombPacket(packet);
        else if (packet->getId() == PACKET_EXPLODE_BOMB)
            parseExplodeBombPacket(packet);
        else if (packet->getId() == PACKET_FALL_PLAYER)
            parseFallPlayerPacket(packet);
        else if (packet->getId() == PACKET_SHUTDOWN)
            parseShutdownPacket(packet);

        routePacket(packet);

        delete packet;
    }
}

void GhostGame::routePacket(Packet* packet)
{
    for (std::map<int, Player*>::iterator it = players.begin(); it != players.end(); ++it)
    {
        Player* player = it->second;
        if (player->getSocket() != NULL)
            player->getSocket()->addOutPacket(packet->clone());
    }
}

void GhostGame::parseAddPlayerPacket(Packet* packet)
{
    int id = Packet::getInt(packet->getData());
    int x = Packet::getShort(packet->getData() + ID_SIZE);
    int y = Packet::getShort(packet->getData() + ID_SIZE + 2);
    char name[NAME_SIZE];
    strcpy(name, packet->getData() + ID_SIZE + 4);

    Player* player = new Player(NULL);
    createPlayerBody(player);

    b2Vec2 position;
    position.Set(x, y);
    player->getBody()->SetTransform(position, 0.0f);

    players.insert(std::make_pair(id, player));
}

void GhostGame::parseRemovePlayerPacket(Packet* packet)
{
    int id = Packet::getInt(packet->getData());
    Player* player = players[id];
    delete player;
    players.erase(id);
}

void GhostGame::parseMovePlayerPacket(Packet* packet)
{
    int id = Packet::getInt(packet->getData());
    int x = Packet::getShort(packet->getData() + ID_SIZE);
    int y = Packet::getShort(packet->getData() + ID_SIZE + 2);

    Player* player = players[id];

    b2Vec2 position;
    position.Set(x, y);
    player->getBody()->SetTransform(position, 0.0f);
}

void GhostGame::parseAddBombPacket(Packet* packet)
{
    int id = Packet::getInt(packet->getData());
    int x = Packet::getShort(packet->getData() + ID_SIZE);
    int y = Packet::getShort(packet->getData() + ID_SIZE + 2);

    Bomb* bomb = new Bomb(id);
    Player* player = players[id];

    createBombBody(bomb, player);

    b2Vec2 position;
    position.Set(x, y);
    bomb->getBody()->SetTransform(position, 0.0f);

    bombs.insert(std::make_pair(id, bomb));
}

void GhostGame::parseExplodeBombPacket(Packet* packet)
{
    int id = Packet::getInt(packet->getData());
    Bomb* bomb = bombs[id];
    delete bomb;
    bombs.erase(id);
}

void GhostGame::parseFallPlayerPacket(Packet* packet)
{
    // Do nothing.
}

void GhostGame::parseShutdownPacket(Packet* packet)
{
    // Do nothing for now.
}
