#ifndef WORLD_GAME_H
#define WORLD_GAME_H

#include "game.h"
#include "server.h"

#define SHUTDOWN_TIME 700000000

class WorldGame : public Game
{
public:

            WorldGame();
            WorldGame(b2World);
           ~WorldGame();
    void    connectionHandler(int, Socket*);
    void    update(float, float, float);
    void    updateServers();

private:

    std::vector<Server*> servers;
    LL                   startupTime;

    void    reconnectPlayer(Player*);
    void    sendGameStateToServer(Socket*);
    void    explodePlayerBombs(int);
    void    broadcastPacketToServers(Packet*);
    void    updateShutdown();
    void    updatePlayerPackets(Player*);
    void    updateServerPackets(Server*);
    void    updatePlayerMovement(Player*);
    void    fallPlayer(Player*);
    void    explodeBomb(Bomb*);
    void    parseLoginPacket(Packet*, Player*);
    void    parseMoveMePacket(Packet*, Player*, int);
    void    parsePlantBombPacket(Packet*, Player*, int);
    void    parseAddPlayerPacket(Packet*, Server*);
    void    parseRemovePlayerPacket(Packet*, Server*);
    void    parseLoginExPacket(Packet*);
    void    parseMoveMeExPacket(Packet*);
    void    parsePlantBombExPacket(Packet*);
    Packet* createMovePlayerPacket(Player*);
    Packet* createExplodeBombPacket(Bomb*);
    Packet* createFallPlayerPacket(Player*);
    Packet* createShutdownPacket();
    Packet* createAddPlayerExPacket(Player*);

};

#endif
