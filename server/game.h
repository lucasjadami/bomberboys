#ifndef GAME_H
#define GAME_H

#include "player.h"
#include "socket.h"
#include <vector>
#include <Box2D/Dynamics/b2World.h>

#define MAP_WIDTH   600
#define MAP_HEIGHT  420

class Game
{
public:

            Game();
           ~Game();
    void    createWorld();
    void    connectionHandler(int, Socket*);
    void    update();

private:

    std::vector<Player*> players;
    b2World*             world;

    void    updatePlayerPackets(Player*);
    void    updatePlayerMovement(Player*);
    void    parseLoginPacket(Packet*, Player*);
    Packet* createAddPlayerPacket(Player*);
    Packet* createRemovePlayerPacket(int);
    Packet* createMovePlayerPacket(Player*);
    void    putBytes(char*, int, int);

};

#endif
