package com.bomberboys;

import com.bomberboys.game.Game;
import com.bomberboys.ui.PlayerKeyboardListener;
import com.bomberboys.ui.MainWindow;
import java.awt.KeyboardFocusManager;

public class Main {
    public static void main(String[] args) throws Exception {
        if (args.length == 3) {
            if (args[0].equals("b")) {
                startBot();
            } else {
                startHuman();
            }
        } else {
            System.out.println("Usage: client [h|b] ip port");
        }
    }

    private static void startHuman() {
        Game game = new Game();
        game.connect();

        MainWindow mainWindow = new MainWindow();
        mainWindow.setVisible(true);

        PlayerKeyboardListener listener = new PlayerKeyboardListener(game);
        KeyboardFocusManager.getCurrentKeyboardFocusManager().addKeyEventDispatcher(listener);

        while (true) {
            try {
                Thread.sleep(50);
            } catch (Exception ex) { }

            game.processPackets();
        }
    }

    private static void startBot() {
        Game game = new Game();
        game.connect();

        while (true) {
            try {
                Thread.sleep(50);
            } catch (Exception ex) { }

            game.processPackets();
            game.doRandomAction();
        }
    }
}
