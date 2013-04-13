#ifndef PACKET_H
#define PACKET_H

#define NAME_SIZE       20
#define PACKET_UID_SIZE 4
#define TRASH_SIZE      0

#define PACKET_LOGIN            0
#define PACKET_ADD_PLAYER       1
#define PACKET_REMOVE_PLAYER    2
#define PACKET_MOVE_ME          3
#define PACKET_MOVE_PLAYER      4
#define PACKET_PLANT_BOMB       5
#define PACKET_ADD_BOMB         6
#define PACKET_EXPLODE_BOMB     7
#define PACKET_FALL_PLAYER      8
#define PACKET_ACK              9
#define PACKET_PING             10
#define PACKET_PONG             11
#define PACKET_INFO             12
#define PACKET_SHUTDOWN         13

#define PACKET_LOGIN_SIZE           NAME_SIZE + TRASH_SIZE
#define PACKET_ADD_PLAYER_SIZE      2 + 2 + 2 + NAME_SIZE + TRASH_SIZE
#define PACKET_REMOVE_PLAYER_SIZE   2 + TRASH_SIZE
#define PACKET_MOVE_ME_SIZE         1 + TRASH_SIZE
#define PACKET_MOVE_PLAYER_SIZE     2 + 2 + 2 + TRASH_SIZE
#define PACKET_PLANT_BOMB_SIZE      0 + TRASH_SIZE
#define PACKET_ADD_BOMB_SIZE        2 + 2 + 2 + TRASH_SIZE
#define PACKET_EXPLODE_BOMB_SIZE    2 + TRASH_SIZE
#define PACKET_FALL_PLAYER_SIZE     2 + TRASH_SIZE
#define PACKET_ACK_SIZE             0 + TRASH_SIZE
#define PACKET_PING_SIZE            0 + TRASH_SIZE
#define PACKET_PONG_SIZE            0 + TRASH_SIZE
#define PACKET_INFO_SIZE            4 + 4 + 4 + 4 + TRASH_SIZE
#define PACKET_SHUTDOWN_SIZE        0 + TRASH_SIZE

class Packet
{
public:

    static void     putBytes(char*, int, int);
    static int      getInt(char*);
    static double   getDouble(char*);
                    Packet(int, char*);
                    Packet(int, int, char*);
                   ~Packet();
    int             getUId();
    void            setUId(int);
    int             getId();
    char*           getData();

private:

    int     uId;
    int     id;
    char*   data;

};

#endif
