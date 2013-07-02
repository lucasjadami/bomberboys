package com.bomberboys.network;

import java.io.IOException;
import java.io.InputStream;
import java.net.InetSocketAddress;
import java.net.Socket;
import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.Collections;
import java.util.LinkedList;
import java.util.List;
import java.util.Random;

public class Connection {
    protected static long DISCONNECT_TIME = 1_000_000_000L;

    private Socket socket;

    protected final List<Packet> sendPackets;
    protected final List<Packet> recvPackets;
    private final List<InetSocketAddress> addressList;
    protected boolean connectFailed;
    protected boolean remoteDisconnected;
    private SendThread send;
    private ReceiveThread recv;
    private AckThread ack;
    private byte[] sid;
    private String username;

    public Connection(String username, byte[] sid, List<InetSocketAddress> addressList) {
        this.addressList = addressList;
        this.sid = sid;
        this.username = username;
        sendPackets = Collections.synchronizedList(new LinkedList<Packet>());
        recvPackets = Collections.synchronizedList(new LinkedList<Packet>());
        connectFailed = false;
        remoteDisconnected = false;
    }
    
    public void connect() {
        Thread connectThread = new Thread() {
            @Override
            public void run() {
                while (true) {
                    establishConnection();
                    try {
                        send = new SendThread(socket.getOutputStream(), sendPackets);
                        recv = new ReceiveThread(socket.getInputStream(), recvPackets);
                        ack  = new AckThread(sendPackets);

                        send.start();
                        recv.start();
                        ack.start();

                        send.join();
                        recv.join();
                        ack.deactivate();
                    } catch (Exception e) {
                    }
                }
            }
        };
        connectThread.start();
    }
    
    public boolean isConnected() {
        // If there are received packets to process, it is still connected!
        return !recvPackets.isEmpty() || (!connectFailed && !remoteDisconnected);
    }
    
    public boolean isConnectFailed() {
        return connectFailed;
    }

    public boolean isRemoteDisconnected() {
        return remoteDisconnected;
    }
    
    public boolean waitForDisconnect() throws Exception {
        long startTime = System.nanoTime();
        while (!remoteDisconnected && System.nanoTime() - startTime < DISCONNECT_TIME) {
            Thread.sleep(15);
        }

        return remoteDisconnected;
    }
    
    public Packet recvPacket() {
        return recvPackets.isEmpty() ? null : recvPackets.remove(0);
    }

    public void sendPacket(Packet packet) {
        sendPackets.add(packet);
    }

    private void establishConnection() {
        boolean connectionEstablished = false;
        while (!connectionEstablished) {
            try {
                InetSocketAddress address = getReachableAddress();
                socket = new Socket();
                socket.connect(address);
                socket.setTcpNoDelay(true);
                connectionEstablished = true;
                System.out.println("Successfully connected with " + address.getHostName() + ":" + address.getPort());
            } catch (IOException e) { }
        }

        ByteBuffer buffer = ByteBuffer.allocate(Packet.Id.LOGIN.getSize());
        buffer.put(sid);
        buffer.put(username.getBytes());
        sendPacket(new Packet(Packet.Id.LOGIN, buffer));
    }

    private InetSocketAddress getReachableAddress() throws IOException {
        Random rand = new Random();
        List<InetSocketAddress> remainingAddresses = new ArrayList<>();

        while (true) {
            remainingAddresses.addAll(addressList); 

            while (!remainingAddresses.isEmpty()) {
                int randomIndex = Math.abs(rand.nextInt() % remainingAddresses.size());
                InetSocketAddress address = remainingAddresses.remove(randomIndex);

                if (address.getAddress().isReachable(1000))
                    return address;
            }

            System.out.println("No reachable address, retrying.");
        }
    }
}
