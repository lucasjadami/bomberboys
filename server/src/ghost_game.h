#ifndef GHOST_GAME_H
#define GHOST_GAME_H

#include "game.h"
#include "server.h"

class GhostGame : public Game
{
public:

            GhostGame(Socket*, void (*)(void));
           ~GhostGame();
    void    connectionHandler(int, Socket*);
    void    update(float, float, float);

private:

    Server* server;

    void (*offlineHandler)(void);

    void    updateServerPackets();
    void    updatePlayerPackets(Player*);
    void    routePacketToClients(Packet*);
    void    routePacketToServer(Player*, Packet*);
    Player* parseAddPlayerExPacket(Packet*);
    void    parseRemovePlayerPacket(Packet*);
    void    parseMovePlayerPacket(Packet*);
    void    parseAddBombPacket(Packet*);
    void    parseExplodeBombPacket(Packet*);
    void    parseFallPlayerPacket(Packet*);
    void    parseShutdownPacket(Packet*);
    Packet* createAddPlayerPacket(Player*);

};

#endif
