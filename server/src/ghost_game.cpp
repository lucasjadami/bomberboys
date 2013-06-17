#include "ghost_game.h"
#include "connection.h"

GhostGame::GhostGame(Socket* worldServerSocket, void (*offlineHandler)(void))
{
    this->offlineHandler = offlineHandler;
    server = new Server(worldServerSocket);
}

GhostGame::~GhostGame()
{
    if (server != NULL)
        delete server;
}

void GhostGame::connectionHandler(int eventId, Socket* socket)
{
    if (eventId == EVENT_SERVER_DISCONNECTED)
    {
        delete server;
        server = NULL;

        for (std::map<int, Player*>::iterator it = players.begin(); it != players.end(); ++it)
        {
            Player* player = it->second;
            if (!player->isLocal())
                player->setLastAlive();
        }

        offlineHandler();
    }
    else if (eventId == EVENT_CLIENT_CONNECTED)
    {
        Player* newPlayer = new Player(socket);

        if (server != NULL)
            server->getSocket()->addOutPacket(createAddPlayerPacket(newPlayer));

        players.insert(std::make_pair(socket->getId(), newPlayer));
    }
    else if (eventId == EVENT_CLIENT_DISCONNECTED)
    {
        for (std::map<int, Player*>::iterator it = players.begin(); it != players.end(); ++it)
        {
            Player* player = it->second;
            if (player->isLocal() && player->getId() == socket->getId())
            {
                if (server != NULL)
                    server->getSocket()->addOutPacket(createRemovePlayerPacket(player->getId()));

                player->setSocket(NULL);
                break;
            }
        }
    }
}

void GhostGame::update(float time, float velocityIteration, float positionIterations)
{
    updateServerPackets();

    for (std::map<int, Player*>::iterator it = players.begin(); it != players.end(); ++it)
    {
        Player* player = it->second;
        if (player->isLocal())
            updatePlayerPackets(player);
    }
}

void GhostGame::updateServerPackets()
{
    Packet* packet;
    while (server != NULL && (packet = server->getSocket()->getInPacket()) != NULL)
    {
        if (packet->getId() == PACKET_ADD_PLAYER_EX)
        {
            Player* player = parseAddPlayerExPacket(packet);

            Packet* newPacket = Game::createAddPlayerPacket(player);
            routePacketToClients(newPacket);
            delete newPacket;

            if (player->isLocal())
                sendGameStateToClient(player);
        }
        else
        {
            routePacketToClients(packet);

            if (packet->getId() == PACKET_REMOVE_PLAYER)
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
        }

        delete packet;
    }
}

void GhostGame::updatePlayerPackets(Player* player)
{
    Packet* packet;
    while ((packet = player->getSocket()->getInPacket()) != NULL)
    {
        routePacketToServer(player, packet);
        delete packet;
    }
}

void GhostGame::routePacketToClients(Packet* packet)
{
    for (std::map<int, Player*>::iterator it = players.begin(); it != players.end(); ++it)
    {
        Player* player = it->second;
        if (player->isLocal())
            player->getSocket()->addOutPacket(packet->clone(packet->getId()));
    }
}

void GhostGame::routePacketToServer(Player* player, Packet* packet)
{
    Packet* newPacket = packet->clone(packet->getId() + EX_GAP);
    Packet::putBytes(newPacket->getData(), player->getId(), ID_SIZE);

    if (server != NULL)
        server->getSocket()->addOutPacket(newPacket);
}

Player* GhostGame::parseAddPlayerExPacket(Packet* packet)
{
    int id = Packet::getInt(packet->getData());
    ULL sId = Packet::getULongLong(packet->getData() + ID_SIZE);
    int x = Packet::getShort(packet->getData() + ID_SIZE + SID_SIZE);
    int y = Packet::getShort(packet->getData() + ID_SIZE + SID_SIZE + 2);

    char name[NAME_SIZE];
    memcpy(name, packet->getData() + ID_SIZE + SID_SIZE + 4, sizeof(char) * NAME_SIZE);
    name[NAME_SIZE - 1] = '\0';

    Player* player;
    std::map<int, Player*>::iterator it = players.find(id);
    if (it == players.end())
    {
        player = new Player(id);
        players.insert(std::make_pair(id, player));
        server->getPlayers()->insert(std::make_pair(id, player));
    }
    else
    {
        // The player is local.
        player = it->second;
    }

    player->setSId(sId);
    player->setName(name);

    createPlayerBody(player);

    b2Vec2 position;
    position.Set(x, y);
    player->getBody()->SetTransform(position, 0.0f);

    players.insert(std::make_pair(id, player));
    sessions.insert(std::make_pair(sId, player));

    return player;
}

void GhostGame::parseRemovePlayerPacket(Packet* packet)
{
    int id = Packet::getInt(packet->getData());
    Player* player = players[id];

    if (!player->isLocal())
        server->getPlayers()->erase(id);

    players.erase(id);
    sessions.erase(player->getSId());
    delete player;
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
