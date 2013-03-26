#ifndef GAME_H
#define GAME_H

#include "player.h"
#include "socket.h"
#include "bomb.h"
#include <vector>
#include <map>
#include <Box2D/Dynamics/b2World.h>

#ifdef BLOCKING_TCP_CONNECTION
#include <pthread.h>
#endif

#define MAP_WIDTH   600
#define MAP_HEIGHT  420

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

#ifdef BLOCKING_TCP_CONNECTION
    pthread_mutex_t      mutex;
#endif

    void    updatePlayerPackets(Player*);
    void    updatePlayerMovement(Player*);
    void    fallPlayer(Player*);
    void    explodeBomb(Bomb*);
    void    createPlayerBody(Player*);
    void    createBombBody(Bomb*, Player*);
    void    parseLoginPacket(Packet*, Player*);
    void    parseMoveMePacket(Packet*, Player*);
    void    parsePlantBombPacket(Packet*, Player*);
    Packet* createAddPlayerPacket(Player*);
    Packet* createRemovePlayerPacket(int);
    Packet* createMovePlayerPacket(Player*);
    Packet* createAddBombPacket(Bomb*);
    Packet* createExplodeBombPacket(Bomb*);
    Packet* createFallPlayerPacket(Player*);
    void    putBytes(char*, int, int);

};

#endif
