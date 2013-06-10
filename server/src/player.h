#ifndef PLAYER_H
#define PLAYER_H

#include "socket.h"
#include "game_type.h"
#include <Box2D/Box2D.h>

#define PLAYER_RADIUS 10.0f
#define MOVEMENT_IMPULSE 10.0f

class Player
{
public:

            Player(Socket*);
            Player(int);
           ~Player();
   char*    getName();
   void     setName(char*);
   Socket*  getSocket();
   void     setSocket(Socket*);
   bool     isPlaying();
   b2Body*  getBody();
   void     setBody(b2Body*);
   void     saveLastPosition();
   bool     isLastPositionDifferent();
   void     applyImpulse(b2Vec2&);
   bool     isLocal();
   int      getId();
   ULL      getSId();
   void     setSId(ULL);

private:

    int     id;
    ULL     sId;
    Socket* socket;
    char*   name;
    b2Body* body;
    int     lastX;
    int     lastY;
    LL      lastImpulse;

};

#endif
