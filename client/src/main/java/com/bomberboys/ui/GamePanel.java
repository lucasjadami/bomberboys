package com.bomberboys.ui;

import com.bomberboys.game.Bomb;
import com.bomberboys.game.Game;
import com.bomberboys.game.Player;
import java.awt.Color;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.RenderingHints;
import javax.swing.JPanel;

@SuppressWarnings("serial")
public class GamePanel extends JPanel {
    private Game game;
    
    public GamePanel(Game game) {
        this.game = game;
        setBackground(Color.BLACK);
    }
    
    @Override
    protected void paintComponent(Graphics g) {
        super.paintComponent(g);
        
        Graphics2D g2D = (Graphics2D) g;
        
        g2D.setRenderingHint(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_ON);
        
        for (Bomb bomb: game.getBombs().values()) {
            bomb.draw(g2D);
        }

        for (Player player : game.getPlayers().values()) {
            player.draw(g2D);
        }
    }
}
