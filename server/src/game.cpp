#include "game.h"
#include "packet.h"
#include "connection.h"
#include "output.h"
#include "time.h"
#include <cstdlib>
#include <cstring>
#include <utility>
#include <time.h>
#include <Box2D/Common/b2Math.h>
#include <Box2D/Collision/Shapes/b2PolygonShape.h>
#include <Box2D/Collision/Shapes/b2CircleShape.h>

Game::Game()
{
    world = NULL;
    srand(time(NULL));

    timespec time;
    startupTime = getTimeLL(getTime(&time));
}

Game::~Game()
{
    if (world != NULL)
        delete world;
    for (unsigned int i = 0; i < players.size(); ++i)
        delete players[i];
    for (std::map<int, Bomb*>::iterator it = bombs.begin(); it != bombs.end(); ++it)
        delete it->second;
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
        Bomb* bomb = bombs.find(socket->getId()) == bombs.end() ? NULL : bombs[socket->getId()];
        if (bomb != NULL)
        {
            explodeBomb(bomb);
            bombs.erase(socket->getId());
        }

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

void Game::update(float time, float velocityIterations, float positionIterations)
{
    world->Step(time, velocityIterations, positionIterations);

    for (unsigned int i = 0; i < players.size(); ++i)
    {
        Player* player = players[i];

        updatePlayerPackets(player);

        if (player->isPlaying())
        {
            fallPlayer(player);
            updatePlayerMovement(player);
        }

        if (player->isIdle())
            player->getSocket()->setDisconnectForced(true);
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

    // The shutdown is sent but the server is kept online.
    updateShutdown();
}

b2World* Game::getWorld()
{
    return world;
}

void Game::updateShutdown()
{
    timespec time;
    long long now = getTimeLL(getTime(&time));
    if (now - startupTime > SHUTDOWN_TIME)
    {
        info("Shutdown sent!");

        startupTime = 1LL << 62;

        for (unsigned int i = 0; i < players.size(); ++i)
        {
            Player* player = players[i];
            Packet* packet = createShutdownPacket();
            player->getSocket()->addOutPacket(packet);
        }
    }
}

void Game::updatePlayerPackets(Player* player)
{
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
                parseMoveMePacket(packet, player);
            else if (packet->getId() == PACKET_PLANT_BOMB)
                parsePlantBombPacket(packet, player);
            else if (packet->getId() == PACKET_ACK)
                parseAckPacket(packet, player);
            else if (packet->getId() == PACKET_PING)
                parsePingPacket(packet, player);
            else if (packet->getId() == PACKET_INFO)
                parseInfoPacket(packet, player);
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

void Game::fallPlayer(Player* player)
{
    if (player->getBody()->GetPosition().x < 0.0f - PLAYER_RADIUS || player->getBody()->GetPosition().x > MAP_WIDTH + PLAYER_RADIUS
            || player->getBody()->GetPosition().y < 0.0f - PLAYER_RADIUS || player->getBody()->GetPosition().y > MAP_HEIGHT + PLAYER_RADIUS)
    {
        b2Vec2 position;
        position.Set(MAP_WIDTH / 2, MAP_HEIGHT / 2);
        player->getBody()->SetTransform(position, 0.0f);

        player->saveLastPosition();

        player->getBody()->SetLinearVelocity(b2Vec2(0.0f, 0.0f));

        for (unsigned int i = 0; i < players.size(); ++i)
        {
            if (!players[i]->isPlaying())
                continue;

            Packet* newPacket = createFallPlayerPacket(player);
            players[i]->getSocket()->addOutPacket(newPacket);

            newPacket = createMovePlayerPacket(player);
            players[i]->getSocket()->addOutPacket(newPacket);
        }
    }
}

void Game::explodeBomb(Bomb* bomb)
{
    for (unsigned int i = 0; i < players.size(); ++i)
    {
        Player* player = players[i];

        if (!player->isPlaying())
                continue;

        Packet* newPacket = createExplodeBombPacket(bomb);
        player->getSocket()->addOutPacket(newPacket);

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

    world->DestroyBody(bomb->getBody());
    delete bomb;
}

void Game::createPlayerBody(Player* player)
{
    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.position.Set(MAP_WIDTH / 2, MAP_HEIGHT / 2);

    b2Body* body = world->CreateBody(&bodyDef);

    b2CircleShape dynamicCircle;
    dynamicCircle.m_radius = PLAYER_RADIUS;

    b2FixtureDef fixtureDef;
    fixtureDef.shape = &dynamicCircle;
    fixtureDef.density = 0.01f;
    fixtureDef.friction = 0.3f;
    fixtureDef.restitution = 0.5f;

    body->CreateFixture(&fixtureDef);
    body->SetLinearDamping(0.1f);

    player->setBody(body);
}

void Game::createBombBody(Bomb* bomb, Player* player)
{
    b2BodyDef bodyDef;
    bodyDef.type = b2_staticBody;
    bodyDef.position.Set(player->getBody()->GetPosition().x, player->getBody()->GetPosition().y);

    b2Body* body = world->CreateBody(&bodyDef);

    b2PolygonShape staticPolygon;
    staticPolygon.SetAsBox(BOMB_SIZE / 2, BOMB_SIZE / 2);

    b2FixtureDef fixtureDef;
    fixtureDef.shape = &staticPolygon;
    fixtureDef.density = 0.0f;
    fixtureDef.friction = 0.0f;
    fixtureDef.filter.maskBits = 0;

    body->CreateFixture(&fixtureDef);

    bomb->setBody(body);
}

void Game::parseLoginPacket(Packet* packet, Player* player)
{
    char* name = new char[NAME_SIZE];
    memcpy(name, packet->getData(), sizeof(char) * NAME_SIZE);
    name[NAME_SIZE - 1] = '\0';
    player->setName(name);

    createPlayerBody(player);
    player->saveLastPosition();

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

    for (std::map<int, Bomb*>::iterator it = bombs.begin(); it != bombs.end(); ++it)
    {
        newPacket = createAddBombPacket(it->second);
        player->getSocket()->addOutPacket(newPacket);
    }
}

void Game::parseMoveMePacket(Packet* packet, Player* player)
{
    int direction = packet->getData()[0];

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

void Game::parsePlantBombPacket(Packet* packet, Player* player)
{
    if (bombs.find(player->getSocket()->getId()) == bombs.end())
    {
        Bomb* bomb = new Bomb(player->getSocket()->getId());
        createBombBody(bomb, player);

        bombs.insert(std::make_pair(player->getSocket()->getId(), bomb));

        for (unsigned int i = 0; i < players.size(); ++i)
        {
            if (!players[i]->isPlaying())
                continue;

            Packet* newPacket = createAddBombPacket(bomb);
            players[i]->getSocket()->addOutPacket(newPacket);
        }
    }
}

void Game::parseAckPacket(Packet* packet, Player* player)
{
    player->updateLastAck();
}

void Game::parsePingPacket(Packet* packet, Player* player)
{
    Packet* newPacket = createPongPacket();
    player->getSocket()->addOutPacket(newPacket);
}

void Game::parseInfoPacket(Packet* packet, Player* player)
{
    double average = Packet::getDouble(packet->getData());
    double deviation = Packet::getDouble(packet->getData() + 16);
    int packetsLost = Packet::getInt(packet->getData() + 32);
    info("Player info (id, avg, dev, ploss): %d %lf %lf %d", player->getSocket()->getId(), average, deviation, packetsLost);
}

Packet* Game::createAddPlayerPacket(Player* player)
{
    char* data = new char[PACKET_ADD_PLAYER_SIZE];
    memset(data, 0, sizeof(char) * PACKET_ADD_PLAYER_SIZE);
    Packet::putBytes(data, player->getSocket()->getId(), 2);
    Packet::putBytes(data + 2, player->getBody()->GetPosition().x, 2);
    Packet::putBytes(data + 4, player->getBody()->GetPosition().y, 2);
    memcpy(data + 6, player->getName(), sizeof(char) * NAME_SIZE);
    Packet* packet = new Packet(PACKET_ADD_PLAYER, data);
    return packet;
}

Packet* Game::createRemovePlayerPacket(int id)
{
    char* data = new char[PACKET_REMOVE_PLAYER_SIZE];
    memset(data, 0, sizeof(char) * PACKET_REMOVE_PLAYER_SIZE);
    Packet::putBytes(data, id, 2);
    return new Packet(PACKET_REMOVE_PLAYER, data);
}

Packet* Game::createMovePlayerPacket(Player* player)
{
    char* data = new char[PACKET_MOVE_PLAYER_SIZE];
    memset(data, 0, sizeof(char) * PACKET_MOVE_PLAYER_SIZE);
    Packet::putBytes(data, player->getSocket()->getId(), 2);
    Packet::putBytes(data + 2, (int) player->getBody()->GetPosition().x, 2);
    Packet::putBytes(data + 4, (int) player->getBody()->GetPosition().y, 2);
    return new Packet(PACKET_MOVE_PLAYER, data);
}

Packet* Game::createAddBombPacket(Bomb* bomb)
{
    char* data = new char[PACKET_ADD_BOMB_SIZE];
    memset(data, 0, sizeof(char) * PACKET_ADD_BOMB_SIZE);
    Packet::putBytes(data, bomb->getId(), 2);
    Packet::putBytes(data + 2, (int) bomb->getBody()->GetPosition().x, 2);
    Packet::putBytes(data + 4, (int) bomb->getBody()->GetPosition().y, 2);
    return new Packet(PACKET_ADD_BOMB, data);
}

Packet* Game::createExplodeBombPacket(Bomb* bomb)
{
    char* data = new char[PACKET_EXPLODE_BOMB_SIZE];
    memset(data, 0, sizeof(char) * PACKET_EXPLODE_BOMB_SIZE);
    Packet::putBytes(data, bomb->getId(), 2);
    return new Packet(PACKET_EXPLODE_BOMB, data);
}

Packet* Game::createFallPlayerPacket(Player* player)
{
    char* data = new char[PACKET_FALL_PLAYER_SIZE];
    memset(data, 0, sizeof(char) * PACKET_FALL_PLAYER_SIZE);
    Packet::putBytes(data, player->getSocket()->getId(), 2);
    return new Packet(PACKET_FALL_PLAYER, data);
}

Packet* Game::createPongPacket()
{
    char* data = new char[PACKET_PONG_SIZE];
    return new Packet(PACKET_PONG, data);
}

Packet* Game::createShutdownPacket()
{
    char* data = new char[PACKET_SHUTDOWN_SIZE];
    return new Packet(PACKET_SHUTDOWN, data);
}
