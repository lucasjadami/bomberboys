package com.bomberboys.ui;

import com.bomberboys.game.Bomb;
import com.bomberboys.game.Game;
import com.bomberboys.game.Player;
import java.awt.Color;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.RenderingHints;
import java.awt.image.BufferedImage;
import javax.imageio.ImageIO;
import javax.swing.JPanel;
import java.io.IOException;

@SuppressWarnings("serial")
public class GamePanel extends JPanel {
    private Game game;
    private BufferedImage background;
    private int height, width;
    
    public GamePanel(Game game) {
        this.game = game;
        this.height = Game.MAP_SIZE.height;
        this.width  = Game.MAP_SIZE.width;
        setBackground(Color.BLACK);
    }

    public void setup() throws IOException {
        ClassLoader load = getClass().getClassLoader();

        background = ImageIO.read(load.getResourceAsStream("images/ground1.png"));
    }
    
    @Override
    protected void paintComponent(Graphics g) {
        super.paintComponent(g);
        Graphics2D g2D = (Graphics2D) g;
        g2D.setRenderingHint(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_ON);

        paintBackground(g2D);
        
        for (Bomb bomb: game.getBombs().values()) {
            bomb.draw(g2D);
        }

        for (Player player : game.getPlayers().values()) {
            player.draw(g2D);
        }
    }

    private void paintBackground(Graphics2D g) {
        int imageHeight = background.getHeight();
        int imageWidth  = background.getWidth();
        for (int i = 0; i < height; i += imageHeight) {
            for (int j = 0; j < width; j += imageWidth) {
                g.drawImage(background, j, i, null);
            }
        }
    }
}
