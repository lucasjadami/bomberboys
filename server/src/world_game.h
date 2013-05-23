#ifndef WORLD_GAME_H
#define WORLD_GAME_H

#include "game.h"
#include "server.h"

class WorldGame : public Game
{
public:

            WorldGame();
           ~WorldGame();
    void    connectionHandler(int, Socket*);
    void    update(float, float, float);

private:

    std::vector<Server*> servers;
    long long            startupTime;

    void    broadcastPacketToServers(Packet*);
    void    updateShutdown();
    void    updatePlayerPackets(Player*);
    void    updatePlayerMovement(Player*);
    void    fallPlayer(Player*);
    void    explodeBomb(Bomb*);
    void    parseLoginPacket(Packet*, Player*);
    void    parseMoveMePacket(Packet*, Player*);
    void    parsePlantBombPacket(Packet*, Player*);
    Packet* createMovePlayerPacket(Player*);
    Packet* createAddBombPacket(Bomb*);
    Packet* createExplodeBombPacket(Bomb*);
    Packet* createFallPlayerPacket(Player*);
    Packet* createShutdownPacket();

};

#endif
