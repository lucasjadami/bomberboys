package com.bomberboys.network;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.Socket;
import java.nio.ByteBuffer;

public class TCPConnection extends Connection {
    private Socket socket;
    protected InputStream inputStream;
    protected OutputStream outputStream;

    @Override
    protected void startConnectThread(final String ip, final int port) {
        Thread connectThread = new Thread() {
            @Override
            public void run() {
                try {
                    socket = new Socket(ip, port);
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
    
    @Override
    protected void startSendThread() {
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
    
    @Override
    protected void startRecvThread()
    {
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
