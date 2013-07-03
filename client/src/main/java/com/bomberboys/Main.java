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
import java.util.Arrays;
import java.nio.ByteBuffer;

public class Main {
    public static void main(String[] args) throws Exception {
        if (args.length >= 3) {
            List<InetSocketAddress> addressList = new ArrayList<>();
            for (int i = 2; i < args.length; i++) {
                String[] address = args[i].split(":");
                addressList.add(new InetSocketAddress(address[0], Integer.parseInt(address[1])));
                System.out.println("Address " + args[i] + " added.");
            }

            long random = new Random().nextLong();

            ByteBuffer buffer = ByteBuffer.allocate(8);
            buffer.putLong(random);

            byte[] sid = buffer.array();

            System.out.println("Using sid: " + random);

            Connection connection = new Connection(args[0], sid, addressList);
            Game game = new Game(connection);
            connection.setGame(game);
            connection.connect();

            GameWindow window = new GameWindow(game);
            window.setup();

            if (args[1].equals("b")) {
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
            System.out.println("Usage: client name [h|b] [ip:port]+");
        }
    }
}
