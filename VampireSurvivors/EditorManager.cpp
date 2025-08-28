#include "AstroidsPrivate.h"
#include "EditorManager.h"
#include <imgui.h>
#include <imgui-SFML.h>

EditorManager::EditorManager(WindowManager & windowManager, InputHandler & inputHandler, GameManager & gameManager)
    : mWindowManager(windowManager)
    , mInputHandler(inputHandler)
    , mGameManager(gameManager)
    , mEvent()
    , mShouldShowWindow(true)
    , mCamera(windowManager.GetWindow())
{
    ImGui::SFML::Init(*mWindowManager.GetWindow());
}

//------------------------------------------------------------------------------------------------------------------------

EditorManager::~EditorManager()
{
    ImGui::SFML::Shutdown();
}

//------------------------------------------------------------------------------------------------------------------------

void EditorManager::Update(float deltaTime)
{
    float cameraSpeed = 600.f;
    // 1. Camera controls
    if (mInputHandler.IsKeyHeld(sf::Keyboard::W))
    {
        mCamera.Move(0, -cameraSpeed * deltaTime);
    }
    if (mInputHandler.IsKeyHeld(sf::Keyboard::S))
    {
        mCamera.Move(0, cameraSpeed * deltaTime);
    }
    if (mInputHandler.IsKeyHeld(sf::Keyboard::A))
    {
        mCamera.Move(-cameraSpeed * deltaTime, 0);
    }
    if (mInputHandler.IsKeyHeld(sf::Keyboard::D))
    {
        mCamera.Move(cameraSpeed * deltaTime, 0);
    }

    // 2. Object selection
    //if (mInputHandler.IsMouseButtonJustPressed(sf::Mouse::Left))
    //{
    //    auto mousePos = mWindowManager.GetMouseWorldPosition(camera);
    //    selectedObject = gameManager.GetObjectAtPosition(mousePos);
    //}

    //// 3. Object manipulation (dragging)
    //if (selectedObject && mInputHandler.IsMouseButtonPressed(sf::Mouse::Left))
    //{
    //    auto mousePos = windowManager.GetMouseWorldPosition(camera);
    //    selectedObject->SetPosition(mousePos);
    //}

    //// 4. Object creation/deletion hotkeys
    //if (mInputHandler.IsKeyJustPressed(sf::Keyboard::N)) // new object
    //    gameManager.AddGameObject(GameObject(mousePos));

    //if (selectedObject && mInputHandler.IsKeyJustPressed(sf::Keyboard::Delete))
    //    gameManager.RemoveGameObject(selectedObject);
}

//------------------------------------------------------------------------------------------------------------------------

void EditorManager::RenderOverlay(float deltaTime)
{
    sf::Time dt;
    ImGui::SFML::Update(*mWindowManager.GetWindow(), sf::milliseconds(1));
    ImGui::Begin("Editor Panel", &mShouldShowWindow);
    ImGui::Text("Selected Object:");
    ImGui::End();

    ImGui::SFML::Render(*mWindowManager.GetWindow());
}

//------------------------------------------------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------------------------------------------------