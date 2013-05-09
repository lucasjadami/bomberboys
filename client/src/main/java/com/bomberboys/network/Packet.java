package com.bomberboys.network;

import java.nio.ByteBuffer;

/**
 *
 */
public class Packet
{
    public static final int UID_SIZE = 4;
    public static final int TRASH_SIZE = 0;
    
    public enum Id
    {
        LOGIN(20),
        ADD_PLAYER(26),
        REMOVE_PLAYER(2),
        MOVE_ME(1),
        MOVE_PLAYER(6),
        PLANT_BOMB(0),
        ADD_BOMB(6),
        EXPLODE_BOMB(2),
        FALL_PLAYER(2),
        ACKNOWLEDGE(0),
        PING(0),
        PONG(0),
        INFO(36),
        SHUTDOWN(0);
        
        private int size;
        
        Id(int size)
        {
            this.size = size;
        }

        public int getSize()
        {
            return size + TRASH_SIZE;
        }
    }
    
    private int uId;
    
    private Id id;

    private ByteBuffer data;

    private long creationTime;

    public Packet(int uId, Id id, ByteBuffer data)
    {
        this.uId = uId;
        this.id = id;
        if (data == null)
        {
            data = ByteBuffer.allocate(0);
        }
        data.rewind();
        this.data = data;
        creationTime = System.nanoTime();
    }

    public ByteBuffer serialize()
    {
        data.rewind();
        ByteBuffer buffer = ByteBuffer.allocate(1 + UID_SIZE + data.remaining());
        buffer.putInt(uId);
        buffer.put((byte) (id.ordinal()));
        buffer.put(data);
        data.rewind();
        buffer.rewind();
        return buffer;
    }

    public ByteBuffer getData()
    {
        return data;
    }

    public int getUId()
    {
        return uId;
    }

    public Id getId()
    {
        return id;
    }

    public long getCreationTime()
    {
        return creationTime;
    }

    @Override
    public String toString()
    {
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
