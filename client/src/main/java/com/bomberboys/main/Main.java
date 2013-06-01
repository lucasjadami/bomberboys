package com.bomberboys.main;

import com.bomberboys.main.game.Game;

public class Main {
    public static void main(String[] args) throws Exception {
        startGameThread();
    }

    private static void startGameThread() {
        new Thread() {
            @Override
            public void run() {
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
        }.start();
    }
}
