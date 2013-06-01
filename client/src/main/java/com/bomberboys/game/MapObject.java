package com.bomberboys.game;

import java.awt.Graphics2D;

public abstract class MapObject {
    protected static final int MAP_HEIGHT = Game.MAP_SIZE.height;

    protected int x;
    protected int y;
    
    public MapObject(int x, int y) {
        this.x = x;
        this.y = y;
    }

    public abstract void draw(Graphics2D g2D);
    
    public int getX() {
        return x;
    }

    public void setX(int x) {
        this.x = x;
    }

    public int getY() {
        return y;
    }

    public void setY(int y) {
        this.y = y;
    }
}
