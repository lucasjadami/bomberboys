package com.bomberboys;

import com.bomberboys.game.Game;

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
    }

    private static void startBot() {
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
