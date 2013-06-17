package com.bomberboys.network;

import java.io.IOException;
import java.io.OutputStream;
import java.util.List;

class AckThread extends Thread {
    private List<Packet> packets;
    private boolean activated;

    public AckThread(List<Packet> packets) {
        this.packets = packets;
        activated = true;
    }

    public void deactivate() {
        activated = false;
    }

    @Override
    public void run() {
        while (activated) {
            try {
                Thread.sleep(15);
            } catch (InterruptedException ex) { }
            packets.add(new Packet(Packet.Id.ACK));
        }
    }
}
