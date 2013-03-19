#ifndef PLAYER_H
#define PLAYER_H

#include "socket.h"
#include <Box2D/Box2D.h>

#define PLAYER_RADIUS 10.0f
#define MOVEMENT_IMPULSE 10.0f

class Player
{
public:

            Player(Socket*);
           ~Player();
   char*    getName();
   void     setName(char*);
   Socket*  getSocket();
   bool     isPlaying();
   b2Body*  getBody();
   void     setBody(b2Body*);
   void     saveLastPosition();
   bool     isLastPositionDifferent();
   void     applyImpulse(b2Vec2&);

private:

    Socket*     socket;
    char*       name;
    b2Body*     body;
    int         lastX;
    int         lastY;
    long long   lastImpulse;

};

#endif
