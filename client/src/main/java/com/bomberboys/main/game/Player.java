package com.bomberboys.main.game;


public class Player extends MapObject {
    public static final int RADIUS = 10;

    private String name;
    private boolean player;

    public Player(int x, int y, String name, boolean player) {
        super(x, y);
        this.name = name;
        this.player = player;
    }

    public String getName() {
        return name;
    }

    public boolean isPlayer() {
        return player;
    }
}
