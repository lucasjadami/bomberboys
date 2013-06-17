package com.bomberboys;

import com.bomberboys.game.Game;
import com.bomberboys.network.Connection;
import com.bomberboys.ui.GameWindow;
import com.bomberboys.ui.PlayerKeyboardListener;
import java.awt.KeyboardFocusManager;
import java.net.InetSocketAddress;
import java.util.ArrayList;
import java.util.List;
import java.util.Random;

public class Main {
    public static void main(String[] args) throws Exception {
        if (args.length >= 3) {
            List<InetSocketAddress> addressList = new ArrayList<>();
            for (int i = 1; i < args.length; i++) {
                String[] address = args[i].split(":");
                addressList.add(new InetSocketAddress(address[0], Integer.parseInt(address[1])));
                System.out.println("Address " + args[i] + " added.");
            }

            byte[] sid = new byte[8];
            new Random().nextBytes(sid);
            Connection connection = new Connection("Test", sid, addressList);
            connection.connect();

            Game game = new Game(connection);

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
            System.out.println("Usage: client [h|b] [ip:port]+");
        }
    }
}
