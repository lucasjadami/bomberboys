#ifndef GHOST_GAME_H
#define GHOST_GAME_H

#include "game.h"
#include "server.h"

class GhostGame : public Game
{
public:

           ~GhostGame();
    void    connectionHandler(int, Socket*);
    void    update(float, float, float);

private:

    Server* server;

    void updateServerSocket();
    void routePacket(Packet*);
    void parseAddPlayerPacket(Packet*);
    void parseRemovePlayerPacket(Packet*);
    void parseMovePlayerPacket(Packet*);
    void parseAddBombPacket(Packet*);
    void parseExplodeBombPacket(Packet*);
    void parseFallPlayerPacket(Packet*);
    void parsePongPacket(Packet*);
    void parseShutdownPacket(Packet*);

};

#endif
