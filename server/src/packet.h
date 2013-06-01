#ifndef PACKET_H
#define PACKET_H

#define NAME_SIZE       20
#define ID_SIZE         4
#define EX_GAP          20

#define PACKET_LOGIN            0
#define PACKET_ADD_PLAYER       1
#define PACKET_REMOVE_PLAYER    2
#define PACKET_MOVE_ME          3
#define PACKET_MOVE_PLAYER      4
#define PACKET_PLANT_BOMB       5
#define PACKET_ADD_BOMB         6
#define PACKET_EXPLODE_BOMB     7
#define PACKET_FALL_PLAYER      8
#define PACKET_PING             9
#define PACKET_PONG             10
#define PACKET_SHUTDOWN         11
#define PACKET_LOGIN_EX         PACKET_LOGIN + EX_GAP
#define PACKET_MOVE_ME_EX       PACKET_MOVE_ME + EX_GAP
#define PACKET_PLANT_BOMB_EX    PACKET_PLANT_BOMB + EX_GAP

#define PACKET_LOGIN_SIZE           NAME_SIZE
#define PACKET_ADD_PLAYER_SIZE      ID_SIZE + 2 + 2 + NAME_SIZE
#define PACKET_REMOVE_PLAYER_SIZE   ID_SIZE
#define PACKET_MOVE_ME_SIZE         1
#define PACKET_MOVE_PLAYER_SIZE     ID_SIZE + 2 + 2
#define PACKET_PLANT_BOMB_SIZE      0
#define PACKET_ADD_BOMB_SIZE        ID_SIZE + 2 + 2
#define PACKET_EXPLODE_BOMB_SIZE    ID_SIZE
#define PACKET_FALL_PLAYER_SIZE     ID_SIZE
#define PACKET_PING_SIZE            0
#define PACKET_PONG_SIZE            0
#define PACKET_SHUTDOWN_SIZE        0
#define PACKET_LOGIN_EX_SIZE        ID_SIZE + PACKET_LOGIN_SIZE
#define PACKET_MOVE_ME_EX_SIZE      ID_SIZE + PACKET_MOVE_ME_SIZE
#define PACKET_PLANT_BOMB_EX_SIZE   ID_SIZE + PACKET_PLANT_BOMB_SIZE

class Packet
{
public:

    static int      getSize(int);
    static void     putBytes(char*, int, int);
    static int      getInt(char*);
    static short    getShort(char*);
    static double   getDouble(char*);
                    Packet(int, char*);
                    Packet(int, int, char*);
                   ~Packet();
    int             getId();
    char*           getData();
    Packet*         clone(int);

private:

    int     id;
    char*   data;

};

#endif
