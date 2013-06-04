package com.bomberboys.network;

import java.io.IOException;
import java.io.OutputStream;
import java.util.List;

class SendThread extends Thread {
    private OutputStream stream;
    private List<Packet> packetList;
    private boolean activated;

    public SendThread(OutputStream stream, List<Packet> packetList) {
        this.stream = stream;
        this.packetList = packetList;
        activated = true;
    }

    public void deactivate() {
        activated = false;
    }

    @Override
    public void run() {
        try {
            while (activated) {
                try { Thread.sleep(15); } catch (InterruptedException ex) { }

                while (!packetList.isEmpty()) {
                    Packet packet = packetList.remove(0);
                    stream.write(packet.serialize().array());
                }
            }
        } catch (IOException ex) {
            deactivate();
        }
    }
}
