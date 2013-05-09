package com.bomberboys.main;

import com.bomberboys.main.game.Game;

public class Main {
    public static void main(String[] args) throws Exception {
        System.out.println("Usage: address port\n");
        startGameThread(args[0], Integer.parseInt(args[1]));
    }

    private static void startGameThread(final String address, final int port) {
        new Thread() {
            @Override
            public void run() {
                Game game = new Game(address, port);
                game.connect();
                
                long nextAck = System.currentTimeMillis() + 1000;
                long now = 0;
                
                while (!game.isShutdown()) {
                    now = System.currentTimeMillis();
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
