#pragma once
#include "Camera2D.h"

class EditorManager
{
public:
    EditorManager(WindowManager & windowManager, InputHandler & inputHandler, GameManager & gameManager);
    virtual ~EditorManager();

    void Update(float deltaTime);
    void RenderOverlay(float deltaTime);

    WindowManager & mWindowManager;
    InputHandler & mInputHandler;
    GameManager & mGameManager;
    sf::Event mEvent;
private:
    bool mShouldShowWindow;
    Camera2D mCamera;
};

