package com.bomberboys.ui;

import com.bomberboys.game.Game;
import java.awt.image.BufferedImage;
import java.io.IOException;
import javax.swing.JPanel;
import javax.swing.Timer;
import javax.swing.JFrame;
import javax.imageio.ImageIO;

public class GameWindow {
    private Game game;
    private JFrame frame;
    private GamePanel panel;
    private Timer timer;

    public GameWindow(Game game) {
        this.game = game;
    }

    public void setup() throws IOException {
        setupImages();
        panel = new GamePanel(game);
        panel.setup();
        timer = new Timer(30, new GameUpdater(game, panel));
        timer.start();
        setupFrame();
    }

    private void setupFrame() {
        frame = new JFrame();
        frame.setSize(Game.MAP_SIZE.width, Game.MAP_SIZE.height);
        frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frame.setContentPane(panel);
        frame.setVisible(true);
    }

    private void setupImages() throws IOException {
        ClassLoader load = getClass().getClassLoader();
        Images.put("background", ImageIO.read(load.getResourceAsStream("images/ground1.png")));

        //bombs
        BufferedImage b1 = ImageIO.read(load.getResourceAsStream("images/bomb1.png"));
        Images.put("bomb1", b1);

        BufferedImage b3 = ImageIO.read(load.getResourceAsStream("images/bomb3.png"));
        Images.put("bomb3", b3);


        BufferedImage b = ImageIO.read(load.getResourceAsStream("images/bomb2.png"));
        int s = b.getHeight();

        Images.put("bomb2", getScaledInstance(b, 5*s/6, 5*s/6));
        Images.put("bomb4", getScaledInstance(b, 7*s/6, 7*s/6));

        b.getGraphics().dispose();
    }

    private BufferedImage getScaledInstance(BufferedImage img, int w, int h) {
        BufferedImage newImg = new BufferedImage(w, h, img.getType());
        newImg.getGraphics()
            .drawImage(img.getScaledInstance(w, h, BufferedImage.SCALE_SMOOTH), 0, 0 , null);
        return newImg;
    }
}
