package com.bomberboys.network;

import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.nio.ByteBuffer;
import java.util.Arrays;

public class UDPConnection extends Connection
{
    private static final int BUFFER_SIZE = 1024;
    
    private DatagramSocket socket;
    
    private int port;
    
    private InetAddress address;
    
    @Override
    protected void startConnectThread(final String ip, final int port)
    {
        Thread connectThread = new Thread()
        {
            @Override
            public void run()
            {
                try
                {
                    socket = new DatagramSocket();
                    UDPConnection.this.port = port;
                    UDPConnection.this.address = InetAddress.getByName(ip);
                    startSendThread();
                    startRecvThread();
                }
                catch (Exception ex)
                {
                    connectFailed = true;
                }
            }
        };
        connectThread.start();
    }

    @Override
    protected void startSendThread()
    {
        Thread sendThread = new Thread()
        {
            @Override
            public void run()
            {
                try
                {
                    while (!remoteDisconnected)
                    {
                        try
                        {
                            Thread.sleep(INTERVAL);
                        }
                        catch (InterruptedException ex)
                        {
                        }

                        while (!sendPackets.isEmpty())
                        {
                            Packet packet = sendPackets.remove(0);
                            byte[] buffer = packet.serialize().array();
                            DatagramPacket datagramPacket = new DatagramPacket(
                                    buffer, buffer.length, address, port);
                            socket.send(datagramPacket);
                        }
                    }
                }
                catch (IOException ex)
                {
                    remoteDisconnected = true;
                }
            }
        };
        sendThread.start();
    }

    @Override
    protected void startRecvThread()
    {
        Thread recvThread = new Thread()
        {
            @Override
            public void run()
            {
                try
                {
                    byte[] buffer = new byte[BUFFER_SIZE];
                    int bufferPointer = 0;
                    
                    while (!remoteDisconnected)
                    {
                        DatagramPacket datagramPacket = new DatagramPacket(
                                buffer, buffer.length, address, port);
                        socket.receive(datagramPacket);
                        
                        while (bufferPointer < datagramPacket.getLength())
                        {
                            Packet.Id id = Packet.Id.values()[buffer[bufferPointer + Packet.UID_SIZE]];

                            byte[] dataArray = Arrays.copyOfRange(buffer, 
                                    bufferPointer + 1 + Packet.UID_SIZE, bufferPointer + 1 + Packet.UID_SIZE + id.getSize());
                            Packet packet = new Packet(
                                    ByteBuffer.wrap(buffer).getInt(), id, ByteBuffer.wrap(dataArray));
                            recvPackets.add(packet);
                            bufferPointer += id.getSize() + Packet.UID_SIZE + 1;
                        }
                        bufferPointer = 0;
                    }
                }
                catch (IOException ex)
                {
                    remoteDisconnected = true;
                }
            }
        };
        recvThread.start();
    }
}
