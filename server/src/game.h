#ifndef GAME_H
#define GAME_H

#include "player.h"
#include "socket.h"
#include "bomb.h"
#include <map>
#include <Box2D/Dynamics/b2World.h>

#define MAP_WIDTH   600
#define MAP_HEIGHT  420

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

    void            createPlayerBody(Player*);
    void            createBombBody(Bomb*, Player*);
    void            parsePingPacket(Packet*, Player*);
    Packet*         createRemovePlayerPacket(int);
    Packet*         createPongPacket();
    Packet*         createAddBombPacket(Bomb*);
    virtual Packet* createAddPlayerPacket(Player*);

};

#endif
