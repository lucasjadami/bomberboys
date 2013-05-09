#ifndef GAME_H
#define GAME_H

#include "player.h"
#include "socket.h"
#include "bomb.h"
#include <vector>
#include <map>
#include <Box2D/Dynamics/b2World.h>

#define MAP_WIDTH   600
#define MAP_HEIGHT  420

#define SHUTDOWN_TIME 70000

class Game
{
public:

             Game();
            ~Game();
    void     createWorld();
    void     connectionHandler(int, Socket*);
    void     update(float, float, float);
    b2World* getWorld();

private:

    std::vector<Player*> players;
    std::map<int, Bomb*> bombs;
    b2World*             world;
    long long            startupTime;

    void    updateShutdown();
    void    updatePlayerPackets(Player*);
    void    updatePlayerMovement(Player*);
    void    fallPlayer(Player*);
    void    explodeBomb(Bomb*);
    void    createPlayerBody(Player*);
    void    createBombBody(Bomb*, Player*);
    void    parseLoginPacket(Packet*, Player*);
    void    parseMoveMePacket(Packet*, Player*);
    void    parsePlantBombPacket(Packet*, Player*);
    void    parseAckPacket(Packet*, Player*);
    void    parsePingPacket(Packet*, Player*);
    void    parseInfoPacket(Packet*, Player*);
    Packet* createAddPlayerPacket(Player*);
    Packet* createRemovePlayerPacket(int);
    Packet* createMovePlayerPacket(Player*);
    Packet* createAddBombPacket(Bomb*);
    Packet* createExplodeBombPacket(Bomb*);
    Packet* createFallPlayerPacket(Player*);
    Packet* createPongPacket();
    Packet* createShutdownPacket();

};

#endif
