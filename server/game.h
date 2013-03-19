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
    std::map<int, Bomb*> bombs;
    b2World*             world;

    void    updatePlayerPackets(Player*);
    void    updatePlayerMovement(Player*);
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
    void    putBytes(char*, int, int);

};

#endif
