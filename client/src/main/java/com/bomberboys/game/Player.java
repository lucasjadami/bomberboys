package com.bomberboys.game;

import java.awt.Color;
import java.awt.Graphics2D;

public class Player extends MapObject {
    public static final int RADIUS = 10;

    private String name;
    private boolean player;

    public Player(int x, int y, String name, boolean player) {
        super(x, y);
        this.name = name;
        this.player = player;
    }

    @Override
    public void draw(Graphics2D g2D) {
        g2D.setColor(player ? Color.GREEN : Color.WHITE);
        g2D.fillArc(x - RADIUS, MAP_HEIGHT - (y + RADIUS), RADIUS * 2, RADIUS * 2, 0, 360);
    }

    public void kill() {
        animationInProgress = false;
    }

    public String getName() {
        return name;
    }

    public boolean isPlayer() {
        return player;
    }
}
