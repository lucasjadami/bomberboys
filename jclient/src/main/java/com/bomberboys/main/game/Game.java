package com.bomberboys.main.game;

import com.bomberboys.network.Connection;
import com.bomberboys.network.Packet;
import com.bomberboys.network.TCPConnection;
import com.bomberboys.network.UDPConnection;
import java.nio.ByteBuffer;
import java.util.HashMap;
import java.util.Map;
import java.util.Random;
import java.util.List;
import java.util.ArrayList;

/**
 *
 */
public class Game
{
    private Connection connection;
    private Map<Integer, Player> players;
    private Map<Integer, Bomb> bombs;
    private int packetUId;
    private String address = "localhost";
    private int port = 10011;
    private boolean isInformer = false;
    private int lostPackets = 0;
    private int currentRecvPacket = -1;

    private long currentPingTime;
    private List<Long> pingTimes;
    private boolean isShutdown;
    
    public Game(String protocol, String address, int port, boolean informer)
    {
        connection = "tcp".equals(protocol) ? new TCPConnection() : new UDPConnection();
        this.address = address;
        this.port = port;
        isInformer = informer;
        players = new HashMap<>();
        bombs = new HashMap<>();
        pingTimes = new ArrayList<>();
        isShutdown = false;
    }

    public boolean isShutdown() {
        return isShutdown;
    }

    public void connect()
    {
        connection.connect(address, port);
        
        ByteBuffer buffer = ByteBuffer.allocate(Packet.Id.LOGIN.getSize());
        buffer.put("Test".getBytes());
        
        connection.sendPacket(new Packet(packetUId++, Packet.Id.LOGIN, buffer));
    }

    public boolean isInformer()
    {
        return isInformer;
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

    public void sendPing()
    {
        ByteBuffer buffer = ByteBuffer.allocate(Packet.Id.PING.getSize());
        connection.sendPacket(new Packet(packetUId++, Packet.Id.PING, buffer));
        currentPingTime = System.nanoTime();
    }
    
    public void processPackets()
    {
        Packet packet;
        while ((packet = connection.recvPacket()) != null)
        {
            if (packet.getUId() > currentRecvPacket)
            {
                lostPackets += packet.getUId() - currentRecvPacket - 1;
                currentRecvPacket = packet.getUId();
            }
            else
            {
                continue;
            }

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
                case PONG:
                    pong(packet.getData()); break;
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
        if (isInformer) {
            ByteBuffer newBuffer = ByteBuffer.allocate(Packet.Id.INFO.getSize());
            newBuffer.putLong(timeMean());
            newBuffer.putLong(1L);
            newBuffer.putLong(timeDev());
            newBuffer.putLong(1L);
            newBuffer.putInt(lostPackets);
            connection.sendPacket(new Packet(packetUId++, Packet.Id.INFO, newBuffer));
        }
        isShutdown = true;
    }

    private void pong(ByteBuffer buffer)
    {
        pingTimes.add((System.nanoTime() - currentPingTime)/1000000L);
    }

    private long timeMean()
    {
        long sum = 0;
        for (Long time : pingTimes)
        {
            sum += time;
        }

        return sum / pingTimes.size();
    }

    private long timeDev()
    {
        return (long) Math.sqrt(timeVariance());
    }

    private long timeVariance()
    {
        long mean = timeMean();
        long sum = 0;
        for (Long time : pingTimes)
        {
            sum += Math.pow(time - mean, 2);
        }

        return sum / (pingTimes.size() - 1);
    }
}
