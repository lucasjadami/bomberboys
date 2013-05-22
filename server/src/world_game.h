#ifndef WORLD_GAME_H
#define WORLD_GAME_H

#include "game.h"

class WorldGame : public Game
{
public:

            WorldGame();
    void    connectionHandler(int, Socket*);
    void    update(float, float, float);

private:

    long long startupTime;

    void    updateShutdown();
    void    updatePlayerPackets(Player*);
    void    updatePlayerMovement(Player*);
    void    fallPlayer(Player*);
    void    explodeBomb(Bomb*);
    void    parseLoginPacket(Packet*, Player*);
    void    parseMoveMePacket(Packet*, Player*);
    void    parsePlantBombPacket(Packet*, Player*);
    void    parsePingPacket(Packet*, Player*);
    Packet* createMovePlayerPacket(Player*);
    Packet* createAddBombPacket(Bomb*);
    Packet* createExplodeBombPacket(Bomb*);
    Packet* createFallPlayerPacket(Player*);
    Packet* createPongPacket();
    Packet* createShutdownPacket();

};

#endif
