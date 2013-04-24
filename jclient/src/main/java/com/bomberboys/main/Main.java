package com.bomberboys.main;

import com.bomberboys.main.game.Game;

/**
 *
 */
public class Main
{
    public static void main(String[] args) throws Exception
    {
        for (int i = 0; i < 50; ++i)
        {
            startGameThread();
        }
    }

    private static void startGameThread()
    {
        new Thread()
        {
            @Override
            public void run()
            {
                Game game = new Game();
                game.connect();
                
                long quitTime = System.currentTimeMillis() + 100 * 1000;
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
                        game.sendAcknowledge();
                        nextAck = now + 1000;
                    }
                }
            }
        }.start();
    }
}
