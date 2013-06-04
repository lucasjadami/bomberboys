package com.bomberboys;

import com.bomberboys.game.Game;
import com.bomberboys.ui.PlayerKeyboardListener;
import com.bomberboys.ui.GameWindow;
import com.bomberboys.network.Connection;
import java.awt.KeyboardFocusManager;
import java.net.InetSocketAddress;
import java.util.List;
import java.util.ArrayList;

public class Main {
    public static void main(String[] args) throws Exception {
        if (args.length == 3) {
            List<InetSocketAddress> addressList = new ArrayList<>();
            addressList.add(new InetSocketAddress(args[1], Integer.parseInt(args[2])));
            Connection connection = new Connection(addressList);
            connection.connect();

            Game game = new Game(connection);
            game.login();

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
