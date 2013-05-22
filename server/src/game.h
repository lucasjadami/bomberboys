#ifndef GAME_H
#define GAME_H

#include "player.h"
#include "socket.h"
#include "bomb.h"
#include <map>
#include <Box2D/Dynamics/b2World.h>

#define MAP_WIDTH   600
#define MAP_HEIGHT  420

#define SHUTDOWN_TIME 70000

class Game
{
public:

                    Game();
    virtual        ~Game();
    void            createWorld();
    virtual void    connectionHandler(int, Socket*);
    virtual void    update(float, float, float);
    b2World*        getWorld();

protected:

    std::map<int, Player*> players;
    std::map<int, Bomb*>   bombs;
    b2World*               world;

    void    createPlayerBody(Player*);
    void    createBombBody(Bomb*, Player*);
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
