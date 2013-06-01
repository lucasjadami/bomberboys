package com.bomberboys.ui;

import com.bomberboys.game.Game;
import javax.swing.JPanel;
import javax.swing.Timer;
import javax.swing.JFrame;
import java.io.IOException;

public class GameWindow {
    private Game game;
    private JFrame frame;
    private GamePanel panel;
    private Timer timer;

    public GameWindow(Game game) {
        this.game = game;
    }

    public void setup() throws IOException {
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
}
