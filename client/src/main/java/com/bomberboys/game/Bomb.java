package com.bomberboys.game;

import java.awt.Color;
import java.awt.Graphics2D;
import java.awt.image.BufferedImage;
import javax.imageio.ImageIO;
import java.io.IOException;
import com.bomberboys.ui.Images;

public class Bomb extends MapObject {
    private static final String[] sprites = {"bomb1", "bomb2", "bomb3", "bomb4"};
    private int state;
    private float accelerate_tax;

    public Bomb(int x, int y) {
        super(x, y);
        accelerate_tax = 1;
        state = 0;
    }

    @Override
    public void draw(Graphics2D g) {
        state = (int)(state + accelerate_tax) % 40;
        accelerate_tax += 0.03;
        BufferedImage sprite = Images.get(sprites[state / 10]);
        int size = sprite.getHeight();
        g.drawImage(sprite, x - size / 2, MAP_HEIGHT - (y + size / 2), null);
    }
}
