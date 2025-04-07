#include "AstroidsPrivate.h"
#include <iostream>
#include <cstdlib>

int main()
{
    WindowManager windowManager;
    bool paused = false;
    sf::Clock clock;
    float fpsTimer = 0.f;
    int frameCount = 0;

    while (windowManager.GetWindow()->isOpen())
    {
        GameManager * pGameManager = new GameManager(windowManager);

        while (windowManager.GetWindow()->isOpen() && !pGameManager->IsGameOver())
        {
            windowManager.PollEvents();

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
            {
                paused = !paused;
                pGameManager->SetPausedState(paused);
            }

            float deltaTime = clock.restart().asSeconds();

            frameCount++;
            fpsTimer += deltaTime;

            if (fpsTimer >= 1.f) // Once every second
            {
                std::cout << "FPS: " << frameCount << std::endl;
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