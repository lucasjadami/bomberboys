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
import java.util.Random;

public class Connection {
    protected static int INTERVAL = 15;
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
                    startSendThread();
                    startRecvThread();
                    try {
                        recvThread.join();
                        sendThread.join();
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

    private void startSendThread() {
        sendThread = new Thread() {
            @Override
            public void run() {
                try {
                    OutputStream outputStream = socket.getOutputStream();
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
        recvThread = new Thread() {
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

            private byte[] readArray(int size) throws IOException {
                InputStream inputStream = socket.getInputStream();
                byte[] array = new byte[size];
                int totalRead = 0;

                while (totalRead < array.length) {
                    int bytesRead = inputStream.read(array, totalRead, array.length - totalRead);
                    if (bytesRead == -1) return null;
                    totalRead += bytesRead;
                }

                return array;
            }
        };
        recvThread.start();
    }

}
