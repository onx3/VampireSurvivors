#include "AstroidsPrivate.h"
#include <iostream>
#include <cstdlib>
#include <curl/curl.h>

int main()
{
    WindowManager windowManager;
    InputHandler inputHandler;
    bool paused = false;
    sf::Clock clock;
    float fpsTimer = 0.f;
    int frameCount = 0;

    while (windowManager.GetWindow()->isOpen())
    {
        GameManager * pGameManager = new GameManager(windowManager, inputHandler);

        while (windowManager.GetWindow()->isOpen() && !pGameManager->IsGameOver())
        {
            windowManager.PollEvents();
            inputHandler.Update();

            if (inputHandler.IsKeyJustPressed(sf::Keyboard::Escape))
            {
                paused = !paused;
                pGameManager->SetPausedState(paused);
            }

            float deltaTime = clock.restart().asSeconds();

            frameCount++;
            fpsTimer += deltaTime;

            if (fpsTimer >= 1.f) // Once every second
            {
                fpsTimer = 0.f;
                frameCount = 0;
            }
            if (!paused)
            {
                pGameManager->Update(deltaTime);
            }
            else
            {
                pGameManager->DebugUpdate(deltaTime);
                clock.restart();
            }
            pGameManager->Render(deltaTime);
        }
        delete pGameManager;
        pGameManager = nullptr;

        bool waitingForRestart = true;
        while (windowManager.GetWindow()->isOpen() && waitingForRestart)
        {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter) || sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
            {
                waitingForRestart = false;
            }
        }
    }

    return 0;
}