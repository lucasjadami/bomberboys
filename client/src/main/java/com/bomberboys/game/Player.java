package com.bomberboys.game;

import com.bomberboys.ui.Images;
import java.awt.AlphaComposite;
import java.awt.Color;
import java.awt.Graphics2D;
import java.awt.image.BufferedImage;
import java.io.IOException;
import javax.imageio.ImageIO;

public class Player extends MapObject {
    public static final int RADIUS = 10;
    private static final String[] spritesA =
        {"bomber-a1", "bomber-a2", "bomber-a3", "bomber-a4", "bomber-a5"};
    private static final String[] spritesC =
        {"bomber-c1", "bomber-c2", "bomber-c3", "bomber-c4", "bomber-c5"};
    private static final String[] spritesDown =
        {"bomber-d1", "bomber-d2", "bomber-d3", "bomber-d4", "bomber-d5"};
    private static final String[] spritesLeft =
        {"bomber-l1", "bomber-l2", "bomber-l3", "bomber-l4", "bomber-l5"};
    private static final String[] spritesRight =
        {"bomber-r1", "bomber-r2", "bomber-r3", "bomber-r4", "bomber-r5"};
    private static final String[] spritesUp =
        {"bomber-u1", "bomber-u2", "bomber-u3", "bomber-u4", "bomber-u5"};

    private String name;
    private boolean player;

    public Player(int x, int y, String name, boolean player) {
        super(x, y);
        this.name = name;
        this.player = player;
    }

    @Override
    public void draw(Graphics2D g) {
        /* g2D.setColor(player ? Color.GREEN : Color.WHITE); */
        /* g2D.fillArc(x - RADIUS, MAP_HEIGHT - (y + RADIUS), RADIUS * 2, RADIUS * 2, 0, 360); */
        BufferedImage sprite = Images.get("bomber-d3");
        int size = sprite.getHeight();
        g.drawImage(sprite, x - size / 2, MAP_HEIGHT - (y + size / 2), null);
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
