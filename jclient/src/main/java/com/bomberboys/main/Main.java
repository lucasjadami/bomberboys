package com.bomberboys.main;

import com.bomberboys.main.game.Game;

/**
 *
 */
public class Main
{
    public static void main(String[] args) throws Exception
    {
        System.out.println("Usage: n_bots [tcp|udp] address port\n");
        int nBots = Integer.parseInt(args[0]);
        String protocol = args[1];
        String address = args[2];
        int port = Integer.parseInt(args[3]);

        startGameThread(protocol, address, port, true);
        for (int i = 1; i < nBots; ++i)
        {
            startGameThread(protocol, address, port, false);
        }
    }

    private static void startGameThread(final String protocol, final String address,
                                        final int port, final boolean informer)
    {
        new Thread()
        {
            @Override
            public void run()
            {
                Game game = new Game(protocol, address, port, informer);
                game.connect();
                
                long quitTime = System.currentTimeMillis() + 60 * 1000;
                long nextAck = System.currentTimeMillis() + 1000;
                long now = 0;
                
                while (now < quitTime)
                {
                    now = System.currentTimeMillis();
                    try
                    {
                        Thread.sleep(50);
                    }
                    catch (Exception ex)
                    {
                    }
                    
                    game.processPackets();
                    game.doRandomAction();

                    if (nextAck < now)
                    {
                        if (game.isInformer())
                        {
                            game.sendPing();
                        }
                        game.sendAcknowledge();
                        nextAck = now + 1000;
                    }
                }
                System.out.println("Ended normally.");
            }
        }.start();
    }
}
