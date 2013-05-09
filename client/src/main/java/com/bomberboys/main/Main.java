package com.bomberboys.main;

import com.bomberboys.main.game.Game;
import com.bomberboys.main.game.Player;

public class Main {
    public static void main(String[] args) throws Exception {
        System.out.println("Usage: address port\n");

        Player player = new Player(args[0], Integer.parseInt(args[1]));

        Thread thread = new Thread(player);
        thread.start();
        thread.join();
    }
}
