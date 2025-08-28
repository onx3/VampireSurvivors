#include "AstroidsPrivate.h"
#include <iostream>
#include <cstdlib>
#include <curl/curl.h>
#include "EditorManager.h"

enum class EEngineMode
{
    Game,
    Editor
};

int main()
{
    WindowManager windowManager;
    InputHandler inputHandler;
    EEngineMode engineMode = EEngineMode::Game;

    sf::Clock clock;

    while (windowManager.GetWindow()->isOpen())
    {
        GameManager * pGameManager = new GameManager(windowManager, inputHandler);
        assert(pGameManager && "GameManager is nullptr!!!");
        EditorManager editorManager = EditorManager(windowManager, inputHandler, *pGameManager);

        while (windowManager.GetWindow()->isOpen() && !pGameManager->IsGameOver())
        {
            windowManager.PollEvents();
            inputHandler.Update();

            // Handle Engine Mode switch first
            if (inputHandler.IsKeyJustPressed(sf::Keyboard::F1))
            {
                if (engineMode == EEngineMode::Game)
                {
                    engineMode = EEngineMode::Editor;
                }
                else
                {
                    engineMode = EEngineMode::Game;
                }
            }

            float deltaTime = clock.restart().asSeconds();

            switch (engineMode)
            {
                case (EEngineMode::Game):
                {
                    windowManager.GetWindow()->setMouseCursorVisible(false);
                    pGameManager->Update(deltaTime);
                    break;
                }
                case (EEngineMode::Editor):
                {
                    windowManager.GetWindow()->setMouseCursorVisible(true);
                    editorManager.Update(deltaTime);
                    break;
                }
                default:
                {
                    break;
                }
            }

            pGameManager->Render(deltaTime);
            if (engineMode == EEngineMode::Editor)
            {
                editorManager.RenderOverlay(deltaTime);
            }
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