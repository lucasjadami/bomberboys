#ifndef PLAYER_H
#define PLAYER_H

#include "socket.h"
#include <Box2D/Box2D.h>

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

private:

    Socket* socket;
    char*   name;
    b2Body* body;
    int     lastX;
    int     lastY;

};

#endif
