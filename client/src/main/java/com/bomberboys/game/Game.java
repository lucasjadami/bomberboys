package com.bomberboys.game;

import com.bomberboys.network.Connection;
import com.bomberboys.network.Packet;
import com.bomberboys.network.TCPConnection;
import java.awt.Dimension;
import java.nio.ByteBuffer;
import java.util.HashMap;
import java.util.Map;
import java.util.Random;

public class Game {
    public static final Dimension MAP_SIZE = new Dimension(600, 420);

    private Connection connection;
    private Map<Integer, Player> players;
    private Map<Integer, Bomb> bombs;
    
    public Game() {
        connection = new TCPConnection();
        players = new HashMap<>();
        bombs = new HashMap<>();
    }

    public void connect(String address, int port) {
        connection.connect(address, port);
        
        ByteBuffer buffer = ByteBuffer.allocate(Packet.Id.LOGIN.getSize());
        buffer.put("Test".getBytes());
        
        connection.sendPacket(new Packet(Packet.Id.LOGIN, buffer));
    }

    public void doRandomAction() {
        int value = new Random().nextInt(10);
        if (value == 9) {
            plantKeyPressed();
        } else {
            movementKeyPressed(value);
        }
    }
    
    public void processPackets() {
        Packet packet;
        while ((packet = connection.recvPacket()) != null) {
            switch (packet.getId()) {
                case ADD_PLAYER: addPlayer(packet.getData()); break;
                case MOVE_PLAYER: movePlayer(packet.getData()); break;
                case REMOVE_PLAYER: removePlayer(packet.getData()); break;
                case ADD_BOMB: addBomb(packet.getData()); break;
                case EXPLODE_BOMB: explodeBomb(packet.getData()); break;
                case SHUTDOWN: shutdown(packet.getData()); break;
            }
        }
    }
    
    public void movementKeyPressed(int direction) {
        ByteBuffer buffer = ByteBuffer.allocate(Packet.Id.MOVE_ME.getSize());
        buffer.put((byte) direction);
        connection.sendPacket(new Packet(Packet.Id.MOVE_ME, buffer));
    }
    
    public void plantKeyPressed() {
        ByteBuffer buffer = ByteBuffer.allocate(Packet.Id.PLANT_BOMB.getSize());
        connection.sendPacket(new Packet(Packet.Id.PLANT_BOMB, buffer));
    }

    public Map<Integer, Player> getPlayers() {
        return players;
    }

    public Map<Integer, Bomb> getBombs() {
        return bombs;
    }
    
    private void addPlayer(ByteBuffer buffer) {
        int id = buffer.getInt();
        int x = buffer.getChar();
        int y = buffer.getChar();
        byte[] name = new byte[20];
        buffer.get(name);
        
        Player newPlayer = new Player(x, y, new String(name), players.isEmpty());
        players.put(id, newPlayer);
    }
    
    private void movePlayer(ByteBuffer buffer) {
        int id = buffer.getInt();
        int x = buffer.getChar();
        int y = buffer.getChar();
        
        Player playerToMove = players.get(id);
        if (playerToMove == null) {
            playerToMove = new Player(0, 0, "?", false);
            players.put(id, playerToMove);
        }

        playerToMove.setX(x);
        playerToMove.setY(y);
    }
    
    private void removePlayer(ByteBuffer buffer) {
        int id = buffer.getInt();
        players.remove(id);
    }
    
    private void addBomb(ByteBuffer buffer) {
        int id = buffer.getInt();
        int x = buffer.getChar();
        int y = buffer.getChar();
        
        Bomb bomb = new Bomb(x, y);
        bombs.put(id, bomb);
    }
    
    private void explodeBomb(ByteBuffer buffer) {
        int id = buffer.getInt();
        bombs.remove(id);
    }
    
    private void shutdown(ByteBuffer buffer) {
    }
}
