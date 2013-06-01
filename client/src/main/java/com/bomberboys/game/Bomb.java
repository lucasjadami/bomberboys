package com.bomberboys.game;

import java.awt.Color;
import java.awt.Graphics2D;

public class Bomb extends MapObject {
    private static final int SIZE = 5;
    
    public Bomb(int x, int y) {
        super(x, y);
    }

    @Override
    public void draw(Graphics2D g2D) {
        g2D.setColor(Color.RED);
        g2D.fillRect(x - SIZE / 2, MAP_HEIGHT - (y + SIZE / 2), SIZE, SIZE);
    }
}
