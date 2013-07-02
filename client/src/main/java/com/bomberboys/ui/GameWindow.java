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
        loadImages();
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

    private void loadImages() throws IOException {
        loadBackgroundImages();
        loadCraterImages();
        loadBombImages();
        loadBomberImages();
    }

    private void loadBackgroundImages() throws IOException {
        ClassLoader load = getClass().getClassLoader();
        Images.put("background", ImageIO.read(load.getResourceAsStream("images/ground1.png")));
    }

    private void loadCraterImages() throws IOException {
        ClassLoader load = getClass().getClassLoader();
        Images.put("crater1", ImageIO.read(load.getResourceAsStream("images/crater1.png")));
    }

    private void loadBomberImages() throws IOException {
        loadBomberImagesWithPrefix("bomber-a");
        loadBomberImagesWithPrefix("bomber-c");
        loadBomberImagesWithPrefix("bomber-d");
        loadBomberImagesWithPrefix("bomber-l");
        loadBomberImagesWithPrefix("bomber-r");
        loadBomberImagesWithPrefix("bomber-u");
    }

    private void loadBomberImagesWithPrefix(String prefix) throws IOException {
        ClassLoader load = getClass().getClassLoader();

        for (int i = 1; i < 6; i++) {
            String path = "images/bomber/" + prefix + i + ".png";
            BufferedImage b = ImageIO.read(load.getResourceAsStream(path));
            Images.put(prefix + i, getScaledInstance(b, 2*b.getWidth(), 2*b.getHeight()));
            b.getGraphics().dispose();
        }
    }

    private void loadBombImages() throws IOException {
        ClassLoader load = getClass().getClassLoader();

        BufferedImage b1 = ImageIO.read(load.getResourceAsStream("images/bomb1.png"));
        BufferedImage b3 = ImageIO.read(load.getResourceAsStream("images/bomb3.png"));
        BufferedImage b = ImageIO.read(load.getResourceAsStream("images/bomb2.png"));

        int dimension = b.getHeight()/2;

        //bombs
        Images.put("bomb1", getScaledInstance(b1, dimension, dimension));
        Images.put("bomb2", getScaledInstance(b, 5*dimension/6, 5*dimension/6));
        Images.put("bomb3", getScaledInstance(b3, dimension, dimension));
        Images.put("bomb4", getScaledInstance(b, 7*dimension/6, 7*dimension/6));

        b1.getGraphics().dispose();
        b3.getGraphics().dispose();
        b.getGraphics().dispose();
    }

    private BufferedImage getScaledInstance(BufferedImage img, int w, int h) {
        BufferedImage newImg = new BufferedImage(w, h, img.getType());
        newImg.getGraphics()
            .drawImage(img.getScaledInstance(w, h, BufferedImage.SCALE_SMOOTH), 0, 0 , null);
        return newImg;
    }
}
