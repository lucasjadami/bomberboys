#include "game.h"
#include "packet.h"
#include "output.h"
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
}

Game::~Game()
{
    if (world != NULL)
        delete world;
    for (std::map<int, Player*>::iterator it = players.begin(); it != players.end(); ++it)
        delete it->second;
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

}

void Game::update(float time, float velocityIterations, float positionIterations)
{

}

b2World* Game::getWorld()
{
    return world;
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

void Game::parsePingPacket(Packet* packet, Player* player)
{
    Packet* newPacket = createPongPacket();
    player->getSocket()->addOutPacket(newPacket);
}

Packet* Game::createRemovePlayerPacket(int id)
{
    char* data = new char[PACKET_REMOVE_PLAYER_SIZE];
    memset(data, 0, sizeof(char) * PACKET_REMOVE_PLAYER_SIZE);
    Packet::putBytes(data, id, ID_SIZE);
    return new Packet(PACKET_REMOVE_PLAYER, data);
}

Packet* Game::createPongPacket()
{
    char* data = new char[PACKET_PONG_SIZE];
    return new Packet(PACKET_PONG, data);
}
