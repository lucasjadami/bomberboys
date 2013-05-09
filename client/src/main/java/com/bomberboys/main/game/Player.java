package com.bomberboys.main.game;

public class Player implements Runnable {
    private String address;
    private int port;

    public Player(String address, int port) {
        this.address = address;
        this.port    = port;
    }

    @Override
    public void run() {
        Game game = new Game(address, port);
        game.connect();

        while (!game.isShutdown()) {
            try { Thread.sleep(50); } catch (Exception ex) { }

            game.processPackets();
            game.doRandomAction();
        }
    }
}
