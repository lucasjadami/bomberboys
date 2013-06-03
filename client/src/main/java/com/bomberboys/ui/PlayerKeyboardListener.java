package com.bomberboys.ui;

import com.bomberboys.game.Game;
import java.awt.KeyEventDispatcher;
import java.awt.event.KeyEvent;

public class PlayerKeyboardListener implements KeyEventDispatcher {
    private Game game;

    public PlayerKeyboardListener(Game game) {
        this.game = game;
    }

    @Override
    public boolean dispatchKeyEvent(KeyEvent e) {
        switch (e.getKeyCode()) {
            case KeyEvent.VK_UP:    game.movementKeyPressed(0); break;
            case KeyEvent.VK_RIGHT: game.movementKeyPressed(2); break;
            case KeyEvent.VK_DOWN:  game.movementKeyPressed(4); break;
            case KeyEvent.VK_LEFT:  game.movementKeyPressed(6); break;
            case KeyEvent.VK_SPACE: game.plantKeyPressed();     break;
        }
        return true;   
    }

}
