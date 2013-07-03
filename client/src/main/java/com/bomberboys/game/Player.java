package com.bomberboys.game;

import com.bomberboys.ui.Images;
import java.awt.AlphaComposite;
import java.awt.Color;
import java.awt.Font;
import java.awt.Graphics2D;
import java.awt.image.BufferedImage;
import java.io.IOException;
import javax.imageio.ImageIO;

public class Player extends MapObject {
    private enum Direction { UP, DOWN, LEFT, RIGHT }

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

    private int state;
    private int accX, accY;
    private int oldX, oldY;
    private BufferedImage currentSprite;

    private Color fontColor;

    public Player(int x, int y, String name, boolean player) {
        super(x, y);
        this.name = name;
        this.player = player;
        accX = 0;
        accY = 0;
        currentSprite = Images.get("bomber-d3");
        state = 0;
        fontColor = player ? Color.GREEN : Color.RED;
    }

    @Override
    public void setX(int x) {
        if (x - oldX > 0) {
            accX += 1;
        } else if (x - oldX < 0) {
            accX -= 1;
        }

        oldX = getX();
        super.setX(x);
    }

    @Override
    public void setY(int y) {
        if (y - oldY > 0) {
            accY += 1;
        } else if (y - oldY < 0) {
            accY -= 1;
        }

        oldY = getY();
        super.setY(y);
    }

    @Override
    public void draw(Graphics2D g) {
        if (isMoving()) {
            currentSprite = getNextSprite();
        }

        drawBomber(g, currentSprite);
        drawName(g);

        if (oldX == getX()) {
            accX = 0;
        } else {
            oldX = getX();
        }

        if (oldY == getY()) {
            accY = 0;
        } else {
            oldY = getY();
        }
    }

    private boolean isMoving() {
        return Math.abs(accX) > 0 || Math.abs(accY) > 0;
    }

    private BufferedImage getNextSprite() {
        String[] currentSprites = spritesDown;
        switch (getDirection()) {
            case UP:    currentSprites = spritesUp;    break;
            case DOWN:  currentSprites = spritesDown;  break;
            case LEFT:  currentSprites = spritesLeft;  break;
            case RIGHT: currentSprites = spritesRight; break;
        }

        state = (state + 1) % 20;

        return Images.get(currentSprites[state / 4]);
    }

    private Direction getDirection() {
        //RIGHT
        if (Math.abs(accX) > Math.abs(accY) && accX > 0) {
            return Direction.RIGHT;
        }

        //LEFT
        if (Math.abs(accX) > Math.abs(accY) && accX < 0) {
            return Direction.LEFT;
        }

        //UP
        if (Math.abs(accX) < Math.abs(accY) && accY > 0) {
            return Direction.UP;
        }

        //DOWN
        if (Math.abs(accX) < Math.abs(accY) && accY < 0) {
            return Direction.DOWN;
        }

        return Direction.DOWN;
    }

    private void drawBomber(Graphics2D g, BufferedImage sprite) {
        int size = sprite.getHeight();
        g.drawImage(sprite, x - size / 2, MAP_HEIGHT - (y + size / 2), null);
    }

    private void drawName(Graphics2D g) {
        g.setFont(new Font("TimesRoman", Font.PLAIN, 15));
        int size = g.getFontMetrics().stringWidth(name);
        g.setColor(fontColor);
        g.drawString(name, x - size / 2 - 10, MAP_HEIGHT - (y + currentSprite.getHeight() / 2) + 60);
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
