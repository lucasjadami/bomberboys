package com.bomberboys.network;

import java.nio.ByteBuffer;

public class Packet {
    public enum Id {
        LOGIN(28),
        ADD_PLAYER(28),
        REMOVE_PLAYER(4),
        MOVE_ME(1),
        MOVE_PLAYER(8),
        PLANT_BOMB(0),
        ADD_BOMB(8),
        EXPLODE_BOMB(4),
        FALL_PLAYER(4),
        SHUTDOWN(0);
        
        private int size;
        
        Id(int size) {
            this.size = size;
        }

        public int getSize() {
            return size;
        }
    }
    
    private Id id;
    private ByteBuffer data;

    public Packet(Id id, ByteBuffer data) {
        this.id = id;
        if (data == null) {
            data = ByteBuffer.allocate(0);
        }
        data.rewind();
        this.data = data;
    }

    public ByteBuffer serialize() {
        data.rewind();
        ByteBuffer buffer = ByteBuffer.allocate(1 + data.remaining());
        buffer.put((byte) (id.ordinal()));
        buffer.put(data);
        data.rewind();
        buffer.rewind();
        return buffer;
    }

    public ByteBuffer getData() {
        return data;
    }

    public Id getId() {
        return id;
    }

    @Override
    public String toString() {
        String result = "";
        byte[] buffer = serialize().array();
        for (int i = 0; i < buffer.length; i++)
        {
            if (i != 0)
            {
                result += "-";
            }
            result += Integer.toString((buffer[i] & 0xff) + 0x100, 16).substring(1);
        }
        return result;
    }
}
