package com.bomberboys.ui;

import java.awt.image.BufferedImage;
import java.util.HashMap;
import java.util.Map;
import java.io.IOException;

public class Images {
    private static Map<String, BufferedImage> images = new HashMap<>();

    public static BufferedImage get(String name) {
        return images.get(name);
    }

    public static void put(String name, BufferedImage image) {
        images.put(name, image);
    }
}
