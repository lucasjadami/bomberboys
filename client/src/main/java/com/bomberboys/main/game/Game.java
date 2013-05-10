package com.bomberboys.main.game;

import com.bomberboys.network.Connection;
import com.bomberboys.network.Packet;
import com.bomberboys.network.TCPConnection;
import java.nio.ByteBuffer;
import java.util.HashMap;
import java.util.Map;
import java.util.Random;
import java.util.List;
import java.util.ArrayList;

public class Game {
    private Connection connection;
    private Map<Integer, PlayerInfo> players;
    private Map<Integer, Bomb> bombs;
    private String address;
    private int port;
    private int currentRecvPacket = -1;

    private boolean isShutdown;
    
    public Game(String address, int port) {
        connection = new TCPConnection();
        this.address = address;
        this.port = port;
        players = new HashMap<>();
        bombs = new HashMap<>();
        isShutdown = false;
    }

    public boolean isShutdown() {
        return isShutdown;
    }

    public void connect() {
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

    public Map<Integer, PlayerInfo> getPlayers() {
        return players;
    }

    public Map<Integer, Bomb> getBombs() {
        return bombs;
    }
    
    private void addPlayer(ByteBuffer buffer) {
        int id = buffer.getChar();
        int x = buffer.getChar();
        int y = buffer.getChar();
        byte[] name = new byte[20];
        buffer.get(name);
        
        PlayerInfo newPlayer = new PlayerInfo(x, y, new String(name), players.isEmpty());
        players.put(id, newPlayer);
    }
    
    private void movePlayer(ByteBuffer buffer) {
        int id = buffer.getChar();
        int x = buffer.getChar();
        int y = buffer.getChar();
        
        PlayerInfo playerToMove = players.get(id);
        if (playerToMove == null) {
            playerToMove = new PlayerInfo(0, 0, "?", false);
            players.put(id, playerToMove);
        }

        playerToMove.setX(x);
        playerToMove.setY(y);
    }
    
    private void removePlayer(ByteBuffer buffer) {
        int id = buffer.getChar();
        players.remove(id);
    }
    
    private void addBomb(ByteBuffer buffer) {
        int id = buffer.getChar();
        int x = buffer.getChar();
        int y = buffer.getChar();
        
        Bomb bomb = new Bomb(x, y);
        bombs.put(id, bomb);
    }
    
    private void explodeBomb(ByteBuffer buffer) {
        int id = buffer.getChar();
        bombs.remove(id);
    }
    
    private void shutdown(ByteBuffer buffer) {
        isShutdown = true;
    }
}
