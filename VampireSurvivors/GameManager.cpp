#include "AstroidsPrivate.h"
#include <cassert>
#include <imgui.h>
#include <stack>
#include <imgui-SFML.h>
#include "SpriteComponent.h"
#include "ControlledMovementComponent.h"
#include "ProjectileComponent.h"
#include "BDConfig.h"
#include "CollisionComponent.h"
#include "HealthComponent.h"
#include "PlayerManager.h"
#include "DropManager.h"
#include "ResourceManager.h"
#include "DungeonManager.h"
#include "CameraManager.h"
#include "BaseManager.h"
#include "LevelManager.h"
#include "AbilityUIManager.h"

namespace
{
    static bool gsDrawHitBoxes = false;
    static int gsAbilityThreshold = 5000;
}

GameManager::GameManager(WindowManager & windowManager)
    : mWindowManager(windowManager)
    , mpWindow(windowManager.GetWindow())
    , mEvent(windowManager.GetEvent())
    , mShowImGuiWindow(false)
    , mRootHandle()
    , mManagers()
    , mSoundPlayed(false)
    , mGameState(EGameState::Running)
    , mNextAbilityScoreThreshold(gsAbilityThreshold)
    , mPhysicsWorld(b2Vec2(0.0f, 0.f))
    , mCollisionListener(this)
    , mPaused(false)
{
    // Order Matters
    {
        AddManager<ResourceManager>();

        InitWindow();
        InitImGui();
        mRootHandle = CreateNewGameObject(ETeam::Neutral, BD::Handle(0));

        AddManager<PlayerManager>();

        //Level Manager
        {
            AddManager<LevelManager>();
            GetManager<LevelManager>()->LoadLevel("../Levels/Level1.json");
        }
        AddManager<CameraManager>();
        AddManager<EnemyAIManager>();
        AddManager<UIManager>();
        AddManager<DropManager>();
        AddManager<AbilityUIManager>();
    }
    

    // End Game
    {
        assert(mFont.loadFromFile("Art/font.ttf"));

        mGameOverText.setFont(mFont);
        mGameOverText.setString("GAME OVER");
        mGameOverText.setCharacterSize(64);
        mGameOverText.setFillColor(sf::Color::Cyan);
        mGameOverText.setPosition(700, 400);

        mRunTimeText.setFont(mFont);
        mRunTimeText.setString("");
        mRunTimeText.setCharacterSize(64);
        mRunTimeText.setFillColor(sf::Color::Cyan);
        mRunTimeText.setPosition(700, 500);

        // Setup Score text
        mScoreText.setFont(mFont);
        mScoreText.setCharacterSize(32);
        mScoreText.setFillColor(sf::Color::Cyan);
        mScoreText.setPosition(700, 600);
    }

    // Box2d
    {
        mPhysicsWorld.SetContactListener(&mCollisionListener);
        mPhysicsWorld.Step(0.f, 0, 0);
    }

    // ResourceManager
    {
        auto * pResourceManager = GetManager<ResourceManager>();
    }
}

//------------------------------------------------------------------------------------------------------------------------

GameManager::~GameManager()
{
    // Clean up managers
    for (auto & manager : mManagers)
    {
        if (manager.second)
        {
            delete manager.second;
            manager.second = nullptr;
        }
    }
    mManagers.clear();

    ImGui::SFML::Shutdown();
    if (ImGui::GetCurrentContext() != nullptr)
    {
        ImGui::DestroyContext();
    }
}

//------------------------------------------------------------------------------------------------------------------------

void GameManager::EndGame()
{
    mGameState = EGameState::EndGame;

    // Notify all managers that the game is ending
    for (auto & manager : mManagers)
    {
        if (manager.second)
        {
            manager.second->OnGameEnd();
        }
    }

    if (mRootHandle != BD::Handle(0))
    {
        CleanUpDestroyedGameObjects(mRootHandle);
        if (auto * pRootObj = GetGameObject(mRootHandle))
        {
            delete pRootObj;
        }
        mPool.Remove(mRootHandle);
        mRootHandle = BD::Handle(0);
    }

    mPhysicsWorld.ClearForces();
    GameOverScreen();
}

//------------------------------------------------------------------------------------------------------------------------

void GameManager::Update(float deltaTime)
{
    // Game Audio
    if (!mSoundPlayed)
    {
        mSound.play();
        mSoundPlayed = true;
    }

    // Ability Check
    {
        int score = 0;
        auto * pUIManager = GetManager<UIManager>();
        if (pUIManager)
        {
            score = pUIManager->GetScore();
        }
        if (mGameState == EGameState::Running && score >= mNextAbilityScoreThreshold)
        {
            mGameState = EGameState::AbilitySelect;
            mNextAbilityScoreThreshold += gsAbilityThreshold;
        }
    }

    switch (mGameState)
    {
        case (EGameState::Running):
        {
            // Physics
            {
                float timeStep = 1.0f / 60.0f; // 60 Hz
                int velocityIterations = 8;
                int positionIterations = 3;
                mPhysicsWorld.Step(timeStep, velocityIterations, positionIterations);
            }

            UpdateGameObjects(deltaTime);

            for (auto & manager : mManagers)
            {
                if (manager.second)
                {
                    manager.second->Update(deltaTime);
                }
            }
            break;
        }
        case (EGameState::Paused):
        {
            break;
        }
        case (EGameState::AbilitySelect):
        {
            break;
        }
        case (EGameState::EndGame):
        {

            break;
        }

        default:
            break;
    }
}

//------------------------------------------------------------------------------------------------------------------------

void GameManager::DebugUpdate(float deltaTime)
{
    if (!mpWindow)
    {
        return;
    }
}

//------------------------------------------------------------------------------------------------------------------------

void GameManager::UpdateGameObjects(float deltaTime)
{
    if (auto * pRootObj = GetGameObject(mRootHandle))
    {
        pRootObj->Update(deltaTime);
        CleanUpDestroyedGameObjects(mRootHandle);
        if (!pRootObj)
        {
            EndGame();
        }
    }
}

//------------------------------------------------------------------------------------------------------------------------

void GameManager::CleanUpDestroyedGameObjects(BD::Handle rootHandle)
{
    GameObject * pRoot = GetGameObject(rootHandle);
    if (!pRoot)
    {
        return;
    }

    std::vector<BD::Handle> objectsToDelete;
    std::vector<BD::Handle> & children = pRoot->GetChildrenHandles();

    for (BD::Handle & childHandle : children)
    {
        CleanUpDestroyedGameObjects(childHandle);

        GameObject * pChild = GetGameObject(childHandle);
        if (pChild && pChild->IsDestroyed())
        {
            objectsToDelete.push_back(childHandle);
        }
    }

    // Remove parent reference before deleting
    if (pRoot->IsDestroyed())
    {
        BD::Handle parentHandle = pRoot->GetParentHandle();
        if (parentHandle != BD::Handle(0))
        {
            GameObject * pParent = GetGameObject(parentHandle);
            if (pParent)
            {
                auto & children = pParent->GetChildrenHandles();
                children.erase(std::remove(children.begin(), children.end(), rootHandle), children.end());
            }
        }

        objectsToDelete.push_back(rootHandle);
    }

    // Delete all objects marked for deletion
    for (BD::Handle handle : objectsToDelete)
    {
        GameObject * pObject = GetGameObject(handle);
        if (pObject)
        {
            delete pObject;
        }
        mPool.Remove(handle);
    }
}

//------------------------------------------------------------------------------------------------------------------------

void GameManager::RenderGameObjectImGui()
{
#if IMGUI_ENABLED()
    static GameObject * pSelectedGameObject = nullptr;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::G))
    {
        mShowImGuiWindow = true;
    }

    if (mShowImGuiWindow && GetGameObject(mRootHandle))
    {
        ImGui::Begin("Game Objects", &mShowImGuiWindow, ImGuiWindowFlags_NoCollapse);

        ImGui::Columns(2, "GameObjectsColumns", true);

        // LEFT SIDE: GameObject Tree
        RenderConCommands();

        ImGui::Text("GameObject Tree");
        ImGui::Separator();

        std::stack<std::pair<GameObject *, int>> stack; // GameObject* + Depth
        stack.push({ GetGameObject(mRootHandle), 0 });

        while (!stack.empty())
        {
            auto [pGameObject, depth] = stack.top();
            stack.pop();

            if (!pGameObject || pGameObject->IsDestroyed())
                continue;

            ImGui::Indent(depth * 10.0f);

            std::string label = "GameObject " + std::to_string(reinterpret_cast<std::uintptr_t>(pGameObject));
            if (ImGui::Selectable(label.c_str(), pSelectedGameObject == pGameObject))
            {
                pSelectedGameObject = pGameObject;
            }

            std::vector<GameObject *> childObjs;
            pGameObject->GetChildren(childObjs);
            for (auto * child : childObjs)
            {
                stack.push({ child, depth + 1 });
            }

            ImGui::Unindent(depth * 10.0f);
        }

        ImGui::NextColumn();

        // RIGHT SIDE: Components of the Selected GameObject
        ImGui::Text("Components");
        ImGui::Separator();

        if (pSelectedGameObject && !pSelectedGameObject->IsDestroyed())
        {
            ImGui::Text("GameObject %p", pSelectedGameObject);

            for (auto * pComponent : pSelectedGameObject->GetAllComponents())
            {
                std::string componentLabel = "" + pComponent->GetClassName();

                if (ImGui::CollapsingHeader(componentLabel.c_str()))
                {
                    pComponent->DebugImGuiComponentInfo();
                }
            }
        }
        else
        {
            ImGui::Text("No GameObject selected or it has been deleted.");
            pSelectedGameObject = nullptr;
        }

        ImGui::Columns(1);

        ImGui::End();
    }
#endif
}

//------------------------------------------------------------------------------------------------------------------------

void GameManager::RenderConCommands()
{
    ImGui::Checkbox("Draw Hitboxes", &gsDrawHitBoxes);
}

//------------------------------------------------------------------------------------------------------------------------

void GameManager::Render(float deltaTime)
{
    mpWindow->clear();

    if (mGameState == EGameState::EndGame)
    {
        mpWindow->draw(mGameOverText);
        mpWindow->draw(mRunTimeText);
        mpWindow->draw(mScoreText);
    }
    else
    {
        for (auto & pManager : mManagers)
        {
            pManager.second->Render(*mpWindow);
        }

        mpWindow->setMouseCursorVisible(mShowImGuiWindow);

        if (auto * pRoot = GetGameObject(mRootHandle))
        {
            mpWindow->draw(*pRoot);
        }
    }

    // GameObject ImGui && Debug mode
    {
        int imGuiTime = int(std::max(deltaTime, 0.0001f));
        ImGui::SFML::Update(*mpWindow, sf::milliseconds(1));

        RenderGameObjectImGui();

        // Ability ImGui
        if (mGameState == EGameState::AbilitySelect)
        {
            mpWindow->setMouseCursorVisible(true);
            auto * pAbilityUIManager = GetManager<AbilityUIManager>();
            if (pAbilityUIManager)
            {
                pAbilityUIManager->DrawAbilitySelectionUI(mGameState);
            }
        }

        // Render ImGui draw data
        ImGui::SFML::Render(*mpWindow);

        // Box2d Collision
        DrawPhysicsDebug(*mpWindow);
    }

    mpWindow->display();
}

//------------------------------------------------------------------------------------------------------------------------

void GameManager::DrawPhysicsDebug(sf::RenderTarget & target)
{
    if (!gsDrawHitBoxes)
    {
        return;
    }

    for (b2Body * body = mPhysicsWorld.GetBodyList(); body; body = body->GetNext())
    {
        for (b2Fixture * fixture = body->GetFixtureList(); fixture; fixture = fixture->GetNext())
        {
            if (fixture->GetType() == b2Shape::e_polygon)
            {
                b2PolygonShape * poly = static_cast<b2PolygonShape *>(fixture->GetShape());
                sf::ConvexShape shape;
                shape.setPointCount(poly->m_count);

                for (int i = 0; i < poly->m_count; ++i)
                {
                    b2Vec2 point = body->GetWorldPoint(poly->m_vertices[i]);
                    shape.setPoint(i, sf::Vector2f(point.x * BD::gsPixelsPerMeter, point.y * BD::gsPixelsPerMeter));
                }

                shape.setFillColor(sf::Color(0, 0, 0, 0));
                shape.setOutlineColor(sf::Color::Green);
                shape.setOutlineThickness(1.f);
                target.draw(shape);
            }
        }
    }
}

//------------------------------------------------------------------------------------------------------------------------

BD::Handle GameManager::CreateNewGameObject(ETeam team, BD::Handle parentHandle)
{
    GameObject * pNewObject = new GameObject(this, team, BD::Handle(0), parentHandle);
    BD::Handle newHandle = mPool.AddObject(pNewObject);

    pNewObject->mHandle = newHandle;

    if (auto * pParent = GetGameObject(parentHandle))
    {
        pParent->AddChild(pNewObject);
    }

    auto spriteComp = std::make_shared<SpriteComponent>(pNewObject, *this);
    pNewObject->AddComponent(spriteComp);

    return newHandle;
}

//------------------------------------------------------------------------------------------------------------------------

GameObject * GameManager::GetGameObject(BD::Handle handle)
{
    return mPool.Get(handle);
}

//------------------------------------------------------------------------------------------------------------------------

void GameManager::RemoveGameObject(BD::Handle handle)
{
    mPool.Remove(handle);
}

//------------------------------------------------------------------------------------------------------------------------

GameObject * GameManager::GetRootGameObject()
{
    return GetGameObject(mRootHandle);
}

//------------------------------------------------------------------------------------------------------------------------

BD::Handle GameManager::GetRootGameObjectHandle()
{
    return mRootHandle;
}

//------------------------------------------------------------------------------------------------------------------------

bool GameManager::IsGameOver() const
{
    if (mGameState == EGameState::EndGame)
    {
        return true;
    }
    return false;
}

//------------------------------------------------------------------------------------------------------------------------

b2World & GameManager::GetPhysicsWorld()
{
    return mPhysicsWorld;
}

//------------------------------------------------------------------------------------------------------------------------

void GameManager::SetPausedState(bool pause)
{
    mPaused = pause;
}

//------------------------------------------------------------------------------------------------------------------------

void GameManager::SetGameState(EGameState state)
{
    mGameState = state;
}

//------------------------------------------------------------------------------------------------------------------------

sf::RenderWindow & GameManager::GetWindow()
{
    assert(mpWindow && "mpWindow is nullptr!");
    return *mpWindow;
}

//------------------------------------------------------------------------------------------------------------------------

const sf::Font & GameManager::GetFont() const
{
    return mFont;
}

//------------------------------------------------------------------------------------------------------------------------

std::vector<BD::Handle> GameManager::GetGameObjectsByTeam(ETeam team)
{
    std::vector<BD::Handle> gameObjectHandles;
    GameObject * pRoot = GetGameObject(mRootHandle);

    if (!pRoot)
    {
        return gameObjectHandles;
    }

    std::stack<BD::Handle> stack;
    stack.push(mRootHandle);

    while (!stack.empty())
    {
        BD::Handle currentHandle = stack.top();
        stack.pop();

        GameObject * pCurrent = GetGameObject(currentHandle);
        if (pCurrent)
        {
            if (!pCurrent->IsDestroyed() && pCurrent->GetTeam() == team)
            {
                gameObjectHandles.push_back(currentHandle);
            }
            auto & childHandles = pCurrent->GetChildrenHandles();
            for (BD::Handle childHandle : childHandles)
            {
                stack.push(childHandle);
            }
        }
    }

    return gameObjectHandles;
}

//------------------------------------------------------------------------------------------------------------------------

void GameManager::InitWindow()
{
    mpWindow->setMouseCursorVisible(false);
}

//------------------------------------------------------------------------------------------------------------------------

void GameManager::InitImGui()
{
    if (ImGui::GetCurrentContext() == nullptr)
    {
        ImGui::CreateContext();
    }

    ImGuiIO & io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    ImGui::StyleColorsDark();

    ImGui::SFML::Init(*mpWindow);
}

//------------------------------------------------------------------------------------------------------------------------

void GameManager::GameOverScreen()
{
    auto * pUIManager = GetManager<UIManager>();
    if (pUIManager)
    {
        char buffer[32];
        snprintf(buffer, sizeof(buffer), "Time Survived: %.1fs", pUIManager->GetRunTime());
        mRunTimeText.setString(buffer);
        mScoreText.setString("Score: " + std::to_string(pUIManager->GetScore()) + "\n" + "Press ENTER to Play Again!");
    }
}

//------------------------------------------------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------------------------------------------------