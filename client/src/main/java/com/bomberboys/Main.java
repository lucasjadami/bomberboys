package com.bomberboys;

import com.bomberboys.game.Game;
import com.bomberboys.ui.PlayerKeyboardListener;
import com.bomberboys.ui.GameWindow;
import java.awt.KeyboardFocusManager;

public class Main {
    public static void main(String[] args) throws Exception {
        if (args.length == 3) {
            Game game = new Game();
            game.connect();

            GameWindow window = new GameWindow(game);
            window.setup();

            if (args[0].equals("b")) {
                while (true) {
                    try { Thread.sleep(50); } catch (Exception ex) { }
                    game.doRandomAction();
                }
            } else {
                PlayerKeyboardListener listener = new PlayerKeyboardListener(game);
                KeyboardFocusManager.getCurrentKeyboardFocusManager()
                                    .addKeyEventDispatcher(listener);
            }
        } else {
            System.out.println("Usage: client [h|b] ip port");
        }
    }
}
