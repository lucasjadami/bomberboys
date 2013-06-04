package com.bomberboys.network;

import java.io.IOException;
import java.io.InputStream;
import java.nio.ByteBuffer;
import java.util.List;

class ReceiveThread extends Thread {
    private InputStream stream;
    private List<Packet> packetList;
    
    public ReceiveThread(InputStream stream, List<Packet> packetList) {
        this.stream = stream;
        this.packetList = packetList;
    }

    @Override
    public void run() {
        boolean remoteDisconnected = false;
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
                packetList.add(packet);
            }
        } catch (IOException ex) {
            remoteDisconnected = true;
        }
    }

    private byte[] readArray(int size) throws IOException {
        byte[] array = new byte[size];
        int totalRead = 0;

        while (totalRead < array.length) {
            int bytesRead = stream.read(array, totalRead, array.length - totalRead);
            if (bytesRead == -1) return null;
            totalRead += bytesRead;
        }

        return array;
    }
}
