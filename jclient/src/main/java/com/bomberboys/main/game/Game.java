package com.bomberboys.main.game;

import com.bomberboys.network.Connection;
import com.bomberboys.network.Packet;
import com.bomberboys.network.TCPConnection;
import com.bomberboys.network.UDPConnection;
import java.nio.ByteBuffer;
import java.util.HashMap;
import java.util.Map;
import java.util.Random;

/**
 *
 */
public class Game
{
    private Connection connection;
    
    private Map<Integer, Player> players;
    
    private Map<Integer, Bomb> bombs;
    
    private int packetUId;
    
    public Game()
    {
        connection = new TCPConnection();
        players = new HashMap<>();
        bombs = new HashMap<>();
    }

    public void connect()
    {
        connection.connect("127.0.0.1", 10011);
        
        ByteBuffer buffer = ByteBuffer.allocate(Packet.Id.LOGIN.getSize());
        buffer.put("Test".getBytes());
        
        connection.sendPacket(new Packet(packetUId++, Packet.Id.LOGIN, buffer));
    }
    
    public void doRandomAction()
    {
        int value = new Random().nextInt(10);
        if (value == 9)
        {
            plantKeyPressed();
        }
        else
        {
            movementKeyPressed(value);
        }
    }
    
    public void sendAcknowledge()
    {
        ByteBuffer buffer = ByteBuffer.allocate(Packet.Id.ACKNOWLEDGE.getSize());
        connection.sendPacket(new Packet(packetUId++, Packet.Id.ACKNOWLEDGE, buffer));
    }
    
    public void processPackets()
    {
        Packet packet;
        while ((packet = connection.recvPacket()) != null)
        {
            switch (packet.getId())
            {
                case ADD_PLAYER:
                    addPlayer(packet.getData()); break;
                case MOVE_PLAYER:
                    movePlayer(packet.getData()); break;
                case REMOVE_PLAYER:
                    removePlayer(packet.getData()); break;
                case ADD_BOMB:
                    addBomb(packet.getData()); break;
                case EXPLODE_BOMB:
                    explodeBomb(packet.getData()); break;
                case SHUTDOWN:
                    shutdown(packet.getData()); break;
            }
        }
    }
    
    public void movementKeyPressed(int direction)
    {
        ByteBuffer buffer = ByteBuffer.allocate(Packet.Id.MOVE_ME.getSize());
        buffer.put((byte) direction);
        connection.sendPacket(new Packet(packetUId++, Packet.Id.MOVE_ME, buffer));
    }
    
    public void plantKeyPressed()
    {
        ByteBuffer buffer = ByteBuffer.allocate(Packet.Id.PLANT_BOMB.getSize());
        connection.sendPacket(new Packet(packetUId++, Packet.Id.PLANT_BOMB, buffer));
    }

    public Map<Integer, Player> getPlayers()
    {
        return players;
    }

    public Map<Integer, Bomb> getBombs()
    {
        return bombs;
    }
    
    private void addPlayer(ByteBuffer buffer)
    {
        int id = buffer.getChar();
        int x = buffer.getChar();
        int y = buffer.getChar();
        byte[] name = new byte[20];
        buffer.get(name);
        
        Player newPlayer = new Player(x, y, new String(name), players.isEmpty());
        players.put(id, newPlayer);
    }
    
    private void movePlayer(ByteBuffer buffer)
    {
        int id = buffer.getChar();
        int x = buffer.getChar();
        int y = buffer.getChar();
        
        Player playerToMove = players.get(id);
        if (playerToMove == null)
        {
            playerToMove = new Player(0, 0, "?", false);
            players.put(id, playerToMove);
        }

        playerToMove.setX(x);
        playerToMove.setY(y);
    }
    
    private void removePlayer(ByteBuffer buffer)
    {
        int id = buffer.getChar();
        players.remove(id);
    }
    
    private void addBomb(ByteBuffer buffer)
    {
        int id = buffer.getChar();
        int x = buffer.getChar();
        int y = buffer.getChar();
        
        Bomb bomb = new Bomb(x, y);
        bombs.put(id, bomb);
    }
    
    private void explodeBomb(ByteBuffer buffer)
    {
        int id = buffer.getChar();
        bombs.remove(id);
    }
    
    private void shutdown(ByteBuffer buffer)
    {
        ByteBuffer newBuffer = ByteBuffer.allocate(Packet.Id.INFO.getSize());
        newBuffer.putLong(1);
        newBuffer.putLong(100);
        newBuffer.putLong(1);
        newBuffer.putLong(100);
        newBuffer.putInt(1090);
        connection.sendPacket(new Packet(packetUId++, Packet.Id.INFO, newBuffer));
    }
}
