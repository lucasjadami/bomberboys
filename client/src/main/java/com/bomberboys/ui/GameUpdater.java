package com.bomberboys.ui;

import com.bomberboys.game.Game;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import javax.swing.JPanel;

public class GameUpdater implements ActionListener {
    private JPanel component;
    private Game game;

    public GameUpdater(Game game, JPanel component) {
        this.game = game;
        this.component = component;
    }

    @Override
    public void actionPerformed(ActionEvent e) {
        game.processPackets();
        component.repaint();
    }
}
