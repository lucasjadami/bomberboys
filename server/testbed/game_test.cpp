#include "game_test.h"

Test* GameTest::Create()
{
    return new GameTest();
}

void GameTest::Step(Settings* settings)
{
    Test::Step(settings);

    m_debugDraw.DrawString(5, m_textLine, "Game test");
    m_textLine += 15;
}
