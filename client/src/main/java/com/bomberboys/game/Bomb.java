package com.bomberboys.game;

import java.awt.AlphaComposite;
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
    private int explosionRadius;
    private int animationTimeout;

    private int explosionStage;

    public Bomb(int x, int y) {
        super(x, y);
        accelerate_tax = 1;
        state = 0;
        explosionStage = 0;
        explosionRadius = 0;
        animationTimeout = 100;
    }

    public void explode() {
        explosionStage = 1;
    }

    @Override
    public void draw(Graphics2D g) {
        if (animationInProgress) {
            switch (explosionStage) {
                case 0: drawBomb(g); break;
                case 1: drawExplosion(g); break;
                case 2: drawRuins(g); break;
            }
        }
    }

    private void drawBomb(Graphics2D g) {
        state = (int)(state + accelerate_tax) % 40;
        accelerate_tax += 0.03;
        BufferedImage sprite = Images.get(sprites[state / 10]);
        int size = sprite.getHeight();
        g.drawImage(sprite, x - size / 2, MAP_HEIGHT - (y + size / 2), null);
    }

    private void drawExplosion(Graphics2D g) {
        explosionRadius += 10;
        if (255 - explosionRadius < 0) {
            explosionStage = 2;
            return;
        }

        g.setColor(new Color(255, 0, 0, 255 - explosionRadius));
        g.fillArc(x - explosionRadius, MAP_HEIGHT - (y + explosionRadius),
                explosionRadius * 2, explosionRadius * 2, 0, 360);

    }

    private void drawRuins(Graphics2D g) {
        BufferedImage sprite = Images.get("crater1");
        int size = sprite.getHeight();
        g.setComposite(AlphaComposite.getInstance(AlphaComposite.SRC_OVER, animationTimeout * 0.01f));
        g.drawImage(sprite, x - size / 2, MAP_HEIGHT - (y + size / 2), null);
        g.setComposite(AlphaComposite.getInstance(AlphaComposite.SRC_OVER, 1));

        animationTimeout -= 1;
        if (animationTimeout < 0) animationInProgress = false;
    }
}
