package com.bomberboys.main.game;

public class PlayerInfo extends MapObject {
    public static final int RADIUS = 10;
    private String name;
    private boolean player;

    public PlayerInfo(int x, int y, String name, boolean player) {
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
