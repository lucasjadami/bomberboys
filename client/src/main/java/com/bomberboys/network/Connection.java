package com.bomberboys.network;

import java.util.Collections;
import java.util.LinkedList;
import java.util.List;
import java.io.IOException;
import java.io.InputStream;
import java.net.Socket;
import java.net.InetSocketAddress;
import java.nio.ByteBuffer;
import java.util.Random;

public class Connection {
    protected static long DISCONNECT_TIME = 1_000_000_000L;

    private Socket socket;

    protected final List<Packet> sendPackets;
    protected final List<Packet> recvPackets;
    private final List<InetSocketAddress> addressList;
    protected boolean connectFailed;
    protected boolean remoteDisconnected;
    private Thread sendThread;
    private Thread recvThread;

    public Connection(List<InetSocketAddress> addressList) {
        this.addressList = addressList;
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
                        sendThread = new SendThread(socket.getOutputStream(), sendPackets);
                        recvThread = new ReceiveThread(socket.getInputStream(), recvPackets);
                    } catch (IOException e) { }

                    sendThread.start();
                    recvThread.start();

                    try {
                        sendThread.join();
                        recvThread.join();
                    } catch (InterruptedException e) { }
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
                socket = new Socket();
                socket.connect(addressList.get(0));
                socket.setTcpNoDelay(true);
                connectionEstablished = true;
            } catch (IOException e) { }
        }
    }
}
