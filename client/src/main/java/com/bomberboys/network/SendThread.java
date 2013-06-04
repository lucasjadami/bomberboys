package com.bomberboys.network;

import java.io.IOException;
import java.io.OutputStream;
import java.util.List;

class SendThread extends Thread {
    private OutputStream stream;
    private List<Packet> packetList;

    public SendThread(OutputStream stream, List<Packet> packetList) {
        this.stream = stream;
        this.packetList = packetList;
    }

    @Override
    public void run() {
        boolean remoteDisconnected = false;

        try {
            while (!remoteDisconnected) {
                try { Thread.sleep(15); } catch (InterruptedException ex) { }

                while (!packetList.isEmpty()) {
                    Packet packet = packetList.remove(0);
                    stream.write(packet.serialize().array());
                }
            }
        } catch (IOException ex) {
            remoteDisconnected = true;
        }
    }
}
