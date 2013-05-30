#include "world_game.h"
#include "time.h"
#include "connection.h"
#include <list>

WorldGame::WorldGame()
{
    srand(time(NULL));

    timespec time;
    startupTime = getTimeLL(getTime(&time));
}

WorldGame::~WorldGame()
{
    for (unsigned int i = 0; i < servers.size(); ++i)
        delete servers[i];
}

void WorldGame::connectionHandler(int eventId, Socket* socket)
{
    if (eventId == EVENT_CLIENT_CONNECTED)
    {
        Player* newPlayer = new Player(socket);
        players.insert(std::make_pair(socket->getId(), newPlayer));
    }
    else if (eventId == EVENT_CLIENT_DISCONNECTED)
    {
        explodePlayerBombs(socket->getId());

        std::map<int, Player*>::iterator it = players.begin();
        while (it != players.end())
        {
            Player* player = it->second;

            if (!player->isLocal())
            {
                it++;
                continue;
            }

            if (player->getId() == socket->getId())
            {
                if (player->isPlaying())
                    world->DestroyBody(player->getBody());
                delete player;
                players.erase(it++);
            }
            else
            {
                if (player->isPlaying())
                    player->getSocket()->addOutPacket(createRemovePlayerPacket(socket->getId()));
                it++;
            }
        }

        for (unsigned int i = 0; i < servers.size(); ++i)
            servers[i]->getSocket()->addOutPacket(createRemovePlayerPacket(socket->getId()));
    }
    else if (eventId == EVENT_SERVER_CONNECTED)
    {
        servers.push_back(new Server(socket));

        for (std::map<int, Player*>::iterator it = players.begin(); it != players.end(); ++it)
        {
            if (!it->second->isPlaying())
                continue;

            socket->addOutPacket(createAddPlayerPacket(it->second));
        }

        for (std::map<int, Bomb*>::iterator it = bombs.begin(); it != bombs.end(); ++it)
            socket->addOutPacket(createAddBombPacket(it->second));
    }
    else if (eventId == EVENT_SERVER_DISCONNECTED)
    {
        for (unsigned int i = 0; i < servers.size(); ++i)
        {
            if (servers[i]->getSocket()->getId() == socket->getId())
            {
                std::list<int> playersToRemove;
                for (std::map<int, Player*>::iterator it = servers[i]->getPlayers()->begin(); it != servers[i]->getPlayers()->end(); ++it)
                {
                    playersToRemove.push_back(it->first);
                    delete it->second;
                    players.erase(it->first);
                }

                for (std::map<int, Player*>::iterator it = players.begin(); it != players.end(); ++it)
                {
                    if (!it->second->isPlaying() || !it->second->isLocal())
                        continue;

                    for (std::list<int>::iterator it2 = playersToRemove.begin(); it2 != playersToRemove.end(); ++it2)
                        it->second->getSocket()->addOutPacket(createRemovePlayerPacket(*it2));
                }

                delete servers[i];
                servers.erase(servers.begin() + i);

                break;
            }
        }
    }
}

void WorldGame::update(float time, float velocityIterations, float positionIterations)
{
    world->Step(time, velocityIterations, positionIterations);

    for (std::map<int, Player*>::iterator it = players.begin(); it != players.end(); ++it)
    {
        Player* player = it->second;

        updatePlayerPackets(player);

        if (player->isPlaying())
        {
            fallPlayer(player);
            updatePlayerMovement(player);
        }
    }

    std::map<int, Bomb*>::iterator it = bombs.begin();
    while (it != bombs.end())
    {
        Bomb* bomb = it->second;
        if (bomb->explode())
        {
            explodeBomb(bomb);
            bombs.erase(it++);
        }
        else
            ++it;
    }

    for (unsigned int i = 0; i < servers.size(); ++i)
        updateServerPackets(servers[i]);

    // The shutdown is sent but the server is kept online.
    updateShutdown();
}

void WorldGame::explodePlayerBombs(int playerId)
{
    Bomb* bomb = bombs.find(playerId) == bombs.end() ? NULL : bombs[playerId];
    if (bomb != NULL)
    {
        explodeBomb(bomb);
        bombs.erase(playerId);
    }
}

void WorldGame::broadcastPacketToServers(Packet* packet)
{
    for (unsigned int i = 0; i < servers.size(); ++i)
        servers[i]->getSocket()->addOutPacket(packet->clone(packet->getId()));
    delete packet;
}

void WorldGame::updateShutdown()
{
    timespec time;
    long long now = getTimeLL(getTime(&time));
    if (now - startupTime > SHUTDOWN_TIME)
    {
        info("Shutdown sent!");

        startupTime = 1LL << 62;

        for (std::map<int, Player*>::iterator it = players.begin(); it != players.end(); ++it)
        {
            Player* player = it->second;

            if (!player->isLocal())
                continue;

            player->getSocket()->addOutPacket(createShutdownPacket());
        }

        broadcastPacketToServers(createShutdownPacket());
    }
}

void WorldGame::updatePlayerPackets(Player* player)
{
    if (!player->isLocal())
        return;

    Packet* packet;
    while ((packet = player->getSocket()->getInPacket()) != NULL)
    {
        if (!player->isPlaying())
        {
            if (packet->getId() == PACKET_LOGIN)
                parseLoginPacket(packet, player);
        }
        else
        {
            if (packet->getId() == PACKET_MOVE_ME)
                parseMoveMePacket(packet, player, 0);
            else if (packet->getId() == PACKET_PLANT_BOMB)
                parsePlantBombPacket(packet, player, 0);
            else if (packet->getId() == PACKET_PING)
                parsePingPacket(packet, player);
        }
        delete packet;
    }
}

void WorldGame::updateServerPackets(Server* server)
{
    Packet* packet;
    while ((packet = server->getSocket()->getInPacket()) != NULL)
    {
        if (packet->getId() == PACKET_ADD_PLAYER)
            parseAddPlayerPacket(packet, server);
        else if (packet->getId() == PACKET_REMOVE_PLAYER)
            parseRemovePlayerPacket(packet, server);
        else if (packet->getId() == PACKET_LOGIN_EX)
            parseLoginExPacket(packet);
        else if (packet->getId() == PACKET_MOVE_ME_EX)
            parseMoveMeExPacket(packet);
        else if (packet->getId() == PACKET_PLANT_BOMB_EX)
            parsePlantBombExPacket(packet);
        delete packet;
    }
}

void WorldGame::updatePlayerMovement(Player* player)
{
    if (player->isLastPositionDifferent())
    {
        for (std::map<int, Player*>::iterator it = players.begin(); it != players.end(); ++it)
        {
            if (!it->second->isPlaying() || !it->second->isLocal())
                continue;

            it->second->getSocket()->addOutPacket(createMovePlayerPacket(player));
        }

        broadcastPacketToServers(createMovePlayerPacket(player));
    }

    player->saveLastPosition();
}

void WorldGame::fallPlayer(Player* player)
{
    if (player->getBody()->GetPosition().x < 0.0f - PLAYER_RADIUS || player->getBody()->GetPosition().x > MAP_WIDTH + PLAYER_RADIUS
            || player->getBody()->GetPosition().y < 0.0f - PLAYER_RADIUS || player->getBody()->GetPosition().y > MAP_HEIGHT + PLAYER_RADIUS)
    {
        b2Vec2 position;
        position.Set(MAP_WIDTH / 2, MAP_HEIGHT / 2);
        player->getBody()->SetTransform(position, 0.0f);

        player->saveLastPosition();

        player->getBody()->SetLinearVelocity(b2Vec2(0.0f, 0.0f));

        for (std::map<int, Player*>::iterator it = players.begin(); it != players.end(); ++it)
        {
            if (!it->second->isPlaying() || !it->second->isLocal())
                continue;

            it->second->getSocket()->addOutPacket(createFallPlayerPacket(player));
            it->second->getSocket()->addOutPacket(createMovePlayerPacket(player));
        }

        broadcastPacketToServers(createFallPlayerPacket(player));
        broadcastPacketToServers(createMovePlayerPacket(player));
    }
}

void WorldGame::explodeBomb(Bomb* bomb)
{
    for (std::map<int, Player*>::iterator it = players.begin(); it != players.end(); ++it)
    {
        Player* player = it->second;

        if (!player->isPlaying())
                continue;

        if (player->isLocal())
            player->getSocket()->addOutPacket(createExplodeBombPacket(bomb));

        b2Vec2 blastDir = player->getBody()->GetWorldCenter() - bomb->getBody()->GetWorldCenter();

        if (blastDir.x == 0 && blastDir.y == 0)
        {
            blastDir.x = rand() % 2 == 0 ? -1 : 1;
            blastDir.y = rand() % 2 == 0 ? -1 : 1;
        }

        float distance = blastDir.Normalize();
        float invDistance = 20.0f / distance;
        float impulseMag = b2Min(900.0f * invDistance * invDistance, BOMB_MAX_IMPULSE);
        player->getBody()->ApplyLinearImpulse(impulseMag * blastDir, player->getBody()->GetWorldCenter());
    }

    broadcastPacketToServers(createExplodeBombPacket(bomb));

    world->DestroyBody(bomb->getBody());
    delete bomb;
}

void WorldGame::parseLoginPacket(Packet* packet, Player* player)
{
    char* name = new char[NAME_SIZE];
    memcpy(name, packet->getData(), sizeof(char) * NAME_SIZE);
    name[NAME_SIZE - 1] = '\0';
    player->setName(name);

    createPlayerBody(player);
    player->saveLastPosition();

    player->getSocket()->addOutPacket(createAddPlayerPacket(player));

    for (std::map<int, Player*>::iterator it = players.begin(); it != players.end(); ++it)
    {
        if (it->second == player || !it->second->isPlaying())
            continue;

        if (it->second->isLocal())
            it->second->getSocket()->addOutPacket(createAddPlayerPacket(player));

        player->getSocket()->addOutPacket(createAddPlayerPacket(it->second));
    }

    for (std::map<int, Bomb*>::iterator it = bombs.begin(); it != bombs.end(); ++it)
        player->getSocket()->addOutPacket(createAddBombPacket(it->second));

    broadcastPacketToServers(createAddPlayerPacket(player));
}

void WorldGame::parseMoveMePacket(Packet* packet, Player* player, int offset)
{
    int direction = packet->getData()[offset];

    b2Vec2 impulse;
    impulse.SetZero();

    switch (direction)
    {
        case 0:
            impulse.y = MOVEMENT_IMPULSE; break;
        case 1:
            impulse.x = MOVEMENT_IMPULSE; impulse.y = MOVEMENT_IMPULSE; break;
        case 2:
            impulse.x = MOVEMENT_IMPULSE; break;
        case 3:
            impulse.x = MOVEMENT_IMPULSE; impulse.y = -MOVEMENT_IMPULSE; break;
        case 4:
            impulse.y = -MOVEMENT_IMPULSE; break;
        case 5:
            impulse.x = -MOVEMENT_IMPULSE; impulse.y = -MOVEMENT_IMPULSE; break;
        case 6:
            impulse.x = -MOVEMENT_IMPULSE; break;
        case 7:
            impulse.x = -MOVEMENT_IMPULSE; impulse.y = MOVEMENT_IMPULSE; break;
    }

    player->applyImpulse(impulse);
}

void WorldGame::parsePlantBombPacket(Packet* packet, Player* player, int offset)
{
    if (bombs.find(player->getId()) == bombs.end())
    {
        Bomb* bomb = new Bomb(player->getId());
        createBombBody(bomb, player);

        bombs.insert(std::make_pair(player->getId(), bomb));

        for (std::map<int, Player*>::iterator it = players.begin(); it != players.end(); ++it)
        {
            if (!it->second->isPlaying() || !it->second->isLocal())
                continue;

            it->second->getSocket()->addOutPacket(createAddBombPacket(bomb));
        }

        broadcastPacketToServers(createAddBombPacket(bomb));
    }
}

void WorldGame::parseAddPlayerPacket(Packet* packet, Server* server)
{
    int id = Packet::getInt(packet->getData());
    Player* newPlayer = new Player(id);
    players.insert(std::make_pair(id, newPlayer));
    server->getPlayers()->insert(std::make_pair(id, newPlayer));
}

void WorldGame::parseRemovePlayerPacket(Packet* packet, Server* server)
{
    int id = Packet::getInt(packet->getData());

    Player* player = players[id];
    delete player;
    players.erase(id);
    server->getPlayers()->erase(id);

    explodePlayerBombs(id);

    broadcastPacketToServers(packet->clone(packet->getId()));

    for (std::map<int, Player*>::iterator it = players.begin(); it != players.end(); ++it)
    {
        if (!it->second->isPlaying() || !it->second->isLocal())
                continue;

        it->second->getSocket()->addOutPacket(packet->clone(packet->getId()));
    }
}

void WorldGame::parseLoginExPacket(Packet* packet)
{
    int id = Packet::getInt(packet->getData());
    Player* player = players[id];

    char* name = new char[NAME_SIZE];
    memcpy(name, packet->getData(), sizeof(char) * NAME_SIZE);
    name[NAME_SIZE - 1] = '\0';
    player->setName(name);

    createPlayerBody(player);
    player->saveLastPosition();

    for (std::map<int, Player*>::iterator it = players.begin(); it != players.end(); ++it)
    {
        if (it->second == player || !it->second->isPlaying() || !it->second->isLocal())
            continue;

        it->second->getSocket()->addOutPacket(createAddPlayerPacket(player));
    }

    broadcastPacketToServers(createAddPlayerPacket(player));
}

void WorldGame::parseMoveMeExPacket(Packet* packet)
{
    int id = Packet::getInt(packet->getData());
    parseMoveMePacket(packet, players[id], ID_SIZE);
}

void WorldGame::parsePlantBombExPacket(Packet* packet)
{
    int id = Packet::getInt(packet->getData());
    parsePlantBombPacket(packet, players[id], ID_SIZE);
}

Packet* WorldGame::createMovePlayerPacket(Player* player)
{
    char* data = new char[PACKET_MOVE_PLAYER_SIZE];
    memset(data, 0, sizeof(char) * PACKET_MOVE_PLAYER_SIZE);
    Packet::putBytes(data, player->getId(), ID_SIZE);
    Packet::putBytes(data + ID_SIZE, (int) player->getBody()->GetPosition().x, 2);
    Packet::putBytes(data + ID_SIZE + 2, (int) player->getBody()->GetPosition().y, 2);
    return new Packet(PACKET_MOVE_PLAYER, data);
}

Packet* WorldGame::createExplodeBombPacket(Bomb* bomb)
{
    char* data = new char[PACKET_EXPLODE_BOMB_SIZE];
    memset(data, 0, sizeof(char) * PACKET_EXPLODE_BOMB_SIZE);
    Packet::putBytes(data, bomb->getId(), ID_SIZE);
    return new Packet(PACKET_EXPLODE_BOMB, data);
}

Packet* WorldGame::createFallPlayerPacket(Player* player)
{
    char* data = new char[PACKET_FALL_PLAYER_SIZE];
    memset(data, 0, sizeof(char) * PACKET_FALL_PLAYER_SIZE);
    Packet::putBytes(data, player->getId(), ID_SIZE);
    return new Packet(PACKET_FALL_PLAYER, data);
}

Packet* WorldGame::createShutdownPacket()
{
    char* data = new char[PACKET_SHUTDOWN_SIZE];
    return new Packet(PACKET_SHUTDOWN, data);
}
