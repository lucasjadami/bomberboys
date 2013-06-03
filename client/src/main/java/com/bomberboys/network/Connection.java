package com.bomberboys.network;

import java.util.Collections;
import java.util.LinkedList;
import java.util.List;

public abstract class Connection {
    protected static int INTERVAL = 15;
    protected static long DISCONNECT_TIME = 1_000_000_000L;

    protected final List<Packet> sendPackets;
    protected final List<Packet> recvPackets;
    protected boolean connectFailed;
    protected boolean remoteDisconnected;

    public Connection() {
        sendPackets = Collections.synchronizedList(new LinkedList<Packet>());
        recvPackets = Collections.synchronizedList(new LinkedList<Packet>());
        connectFailed = false;
        remoteDisconnected = false;
    }
    
    public void connect(String ip, int port) {
        startConnectThread(ip, port);
    }
    
    protected abstract void startConnectThread(final String ip, final int port);
    
    protected abstract void startSendThread();
    
    protected abstract void startRecvThread();
    
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
            Thread.sleep(INTERVAL);
        }

        return remoteDisconnected;
    }
    
    public Packet recvPacket() {
        return recvPackets.isEmpty() ? null : recvPackets.remove(0);
    }

    public void sendPacket(Packet packet) {
        sendPackets.add(packet);
    }
}
