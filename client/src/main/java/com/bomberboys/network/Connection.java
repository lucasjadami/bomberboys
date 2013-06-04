package com.bomberboys.network;

import java.util.Collections;
import java.util.LinkedList;
import java.util.List;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.Socket;
import java.net.InetSocketAddress;
import java.nio.ByteBuffer;

public class Connection {
    protected static int INTERVAL = 15;
    protected static long DISCONNECT_TIME = 1_000_000_000L;

    private Socket socket;
    protected InputStream inputStream;
    protected OutputStream outputStream;

    protected final List<Packet> sendPackets;
    protected final List<Packet> recvPackets;
    private final List<InetSocketAddress> addressList;
    protected boolean connectFailed;
    protected boolean remoteDisconnected;

    public Connection(List<InetSocketAddress> addressList) {
        this.addressList = addressList;
        sendPackets = Collections.synchronizedList(new LinkedList<Packet>());
        recvPackets = Collections.synchronizedList(new LinkedList<Packet>());
        connectFailed = false;
        remoteDisconnected = false;
    }
    
    public void connect() {
        startConnectThread(addressList.get(0));
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

    private void startConnectThread(final InetSocketAddress address) {
        Thread connectThread = new Thread() {
            @Override
            public void run() {
                try {
                    socket = new Socket();
                    socket.connect(address);
                    socket.setTcpNoDelay(true);
                    inputStream = socket.getInputStream();
                    outputStream = socket.getOutputStream();

                    startSendThread();
                    startRecvThread();
                } catch (Exception ex) {
                    connectFailed = true;
                }
            }
        };
        connectThread.start();
    }

    private void startSendThread() {
        Thread sendThread = new Thread() {
            @Override
            public void run() {
                try {
                    while (!remoteDisconnected) {
                        try {
                            Thread.sleep(INTERVAL);
                        } catch (InterruptedException ex) { }

                        while (!sendPackets.isEmpty()) {
                            Packet packet = sendPackets.remove(0);
                            byte[] buffer = packet.serialize().array();
                            outputStream.write(buffer);
                        }
                    }
                } catch (IOException ex) {
                    remoteDisconnected = true;
                }
            }
        };
        sendThread.start();
    }

    private void startRecvThread() {
        Thread recvThread = new Thread() {
            @Override
            public void run() {
                try {
                    while (!remoteDisconnected) {
                        byte[] idArray = readArray(1);
                        if (idArray == null) {
                            remoteDisconnected = true;
                            continue;
                        }

                        Packet.Id id = Packet.Id.values()[idArray[0]];

                        // Protects from reading negative sizes.
                        byte[] dataArray = readArray(Math.max(0, id.getSize()));
                        if (dataArray == null) {
                            remoteDisconnected = true;
                            continue;
                        }

                        Packet packet = new Packet(id, ByteBuffer.wrap(dataArray));
                        recvPackets.add(packet);
                    }
                } catch (IOException ex) {
                    remoteDisconnected = true;
                }
            }
        };
        recvThread.start();
    }

    private byte[] readArray(int size) throws IOException {
        byte[] array = new byte[size];

        int totalRead = 0;
        while (totalRead < array.length) {
            int bytesRead = inputStream.read(array, totalRead, array.length - totalRead);

            if (bytesRead == -1) {
                return null;
            }

            totalRead += bytesRead;
        }

        return array;
    }
}
