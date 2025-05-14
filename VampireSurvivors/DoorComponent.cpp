#include "AstroidsPrivate.h"
#include "DoorComponent.h"
#include <sstream>
#include "CollisionComponent.h"
#include "PlayerManager.h"
#include "InputHandler.h"
#include <imgui.h>

DoorComponent::DoorComponent(GameObject * pOwner, GameManager & gameManager, int doorCost)
    : GameComponent(pOwner, gameManager)
    , mIsOpen(false)
    , mDoorCost(doorCost)
    , mInteractionRange(50.f)
    , mIsPlayerInRange(false)
    , mName("DoorComponent")
{
    if (!mFont.loadFromFile("../../VampireSurvivors/Art/font.ttf"))
    {
        std::cerr << "Failed to load font\n";
    }

    mInteractionText.setFont(mFont);
    mInteractionText.setCharacterSize(15);
    mInteractionText.setFillColor(sf::Color::White);
    char buffer[64];
    std::snprintf(buffer, sizeof(buffer), "Press F to open door [Cost: %d]", mDoorCost);
    mInteractionText.setString(buffer);
}

//------------------------------------------------------------------------------------------------------------------------

void DoorComponent::Update(float deltaTime)
{
    GameManager & gameManager = GetGameManager();
    GameObject & doorObj = GetGameObject();

    auto * pPlayerManager = gameManager.GetManager<PlayerManager>();
    if (!pPlayerManager)
    {
        return;
    }

    auto & players = pPlayerManager->GetPlayers();
    if (players.empty())
    {
        return;
    }

    BD::Handle playerHandle = players[0];
    GameObject * pPlayer = gameManager.GetGameObject(playerHandle);
    if (!pPlayer)
    {
        return;
    }

    float distanceSqr = BD::GetMagnitudeSquared(doorObj.GetPosition(), pPlayer->GetPosition());
    mIsPlayerInRange = (distanceSqr <= mInteractionRange * mInteractionRange);

    if (mIsPlayerInRange && !mIsOpen)
    {
        mInteractionText.setPosition(doorObj.GetPosition().x, doorObj.GetPosition().y - 40.f);

        if (BD::IsKeyJustPressed(sf::Keyboard::F))
        {
            int score = 0;
            auto * pUIManager = gameManager.GetManager<UIManager>();
            if (pUIManager)
            {
                score = pUIManager->GetScore();
            }
            if (score >= mDoorCost)
            {
                pUIManager->RemoveScore(mDoorCost);
                Open();
            }
        }
    }
}

//------------------------------------------------------------------------------------------------------------------------

void DoorComponent::draw(sf::RenderTarget & target, sf::RenderStates states)
{
    if (mIsPlayerInRange && !mIsOpen)
    {
        target.draw(mInteractionText);
    }
}

//------------------------------------------------------------------------------------------------------------------------

void DoorComponent::DebugImGuiComponentInfo()
{
}

//------------------------------------------------------------------------------------------------------------------------

std::string & DoorComponent::GetClassName()
{
    return mName;
}

//------------------------------------------------------------------------------------------------------------------------

void DoorComponent::Open()
{
    if (mIsOpen)
    {
        return;
    }
    mIsOpen = true;
    auto pSpriteComp = GetGameObject().GetComponent<SpriteComponent>().lock();
    if (pSpriteComp)
    {
        ResourceId resId = ResourceId("../../VampireSurvivors/Art/Door/DoorOpen.png");
        auto pTexture = GetGameManager().GetManager<ResourceManager>()->GetTexture(resId);
        if (pTexture)
        {
            pSpriteComp->SetSprite(pTexture);
        }
    }
    auto pColisionComp = GetGameObject().GetComponent<CollisionComponent>().lock();
    if (pColisionComp)
    {
        pColisionComp->SetActive(false);
    }
}

//------------------------------------------------------------------------------------------------------------------------

void DoorComponent::Close()
{
    if (!mIsOpen)
    {
        return;
    }
    mIsOpen = false;
    auto pSpriteComp = GetGameObject().GetComponent<SpriteComponent>().lock();
    if (pSpriteComp)
    {
        ResourceId resId = ResourceId("../../VampireSurvivors/Art/Door/DoorClosed.png");
        auto pTexture = GetGameManager().GetManager<ResourceManager>()->GetTexture(resId);
        if (pTexture)
        {
            pSpriteComp->SetSprite(pTexture);
        }
    }
    auto pColisionComp = GetGameObject().GetComponent<CollisionComponent>().lock();
    if (pColisionComp)
    {
        pColisionComp->SetActive(true);
    }
}

//------------------------------------------------------------------------------------------------------------------------

bool DoorComponent::IsOpen()
{
    return mIsOpen;
}

//------------------------------------------------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------------------------------------------------