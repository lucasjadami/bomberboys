#ifndef PACKET_H
#define PACKET_H

#define NAME_SIZE 20

#define PACKET_LOGIN            0
#define PACKET_ADD_PLAYER       1
#define PACKET_REMOVE_PLAYER    2
#define PACKET_MOVE_ME          3
#define PACKET_MOVE_PLAYER      4
#define PACKET_PLANT_BOMB       5
#define PACKET_ADD_BOMB         6
#define PACKET_EXPLODE_BOMB     7
#define PACKET_FALL_PLAYER      8

#define PACKET_LOGIN_SIZE           NAME_SIZE
#define PACKET_ADD_PLAYER_SIZE      2 + 2 + 2 + NAME_SIZE
#define PACKET_REMOVE_PLAYER_SIZE   2
#define PACKET_MOVE_ME_SIZE         1
#define PACKET_MOVE_PLAYER_SIZE     2 + 2 + 2
#define PACKET_PLANT_BOMB_SIZE      0
#define PACKET_ADD_BOMB_SIZE        2 + 2 + 2
#define PACKET_EXPLODE_BOMB_SIZE    2
#define PACKET_FALL_PLAYER_SIZE     2

class Packet
{
public:

            Packet(int, char*);
           ~Packet();
    int     getId();
    char*   getData();

private:

    int     id;
    char*   data;

};

#endif
