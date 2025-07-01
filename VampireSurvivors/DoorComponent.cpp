#include "AstroidsPrivate.h"
#include "DoorComponent.h"
#include <sstream>
#include "CollisionComponent.h"
#include "PlayerManager.h"
#include "InputHandler.h"
#include <imgui.h>
#include <random>
#include "AudioManager.h"

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

        if (gameManager.mInputHandler.IsKeyJustPressed(sf::Keyboard::F))
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

const std::string & DoorComponent::GetClassName()
{
    return mName;
}

//------------------------------------------------------------------------------------------------------------------------

void DoorComponent::Open()
{
    GameManager & gameManager = GetGameManager();
    if (mIsOpen)
    {
        return;
    }
    mIsOpen = true;
    auto pSpriteComp = GetGameObject().GetComponent<SpriteComponent>().lock();
    if (pSpriteComp)
    {
        ResourceId resId = ResourceId("../../VampireSurvivors/Art/Door/DoorOpen.png");
        auto pTexture = gameManager.GetManager<ResourceManager>()->GetTexture(resId);
        if (pTexture)
        {
            pSpriteComp->SetSprite(pTexture);
            pSpriteComp->GetSprite().setTextureRect(sf::IntRect(0, 0, 32, 32));
            pSpriteComp->GetSprite().setOrigin(16.f, 16.f);
        }
    }
    auto pColisionComp = GetGameObject().GetComponent<CollisionComponent>().lock();
    if (pColisionComp)
    {
        pColisionComp->SetActive(false);
    }

    // Play Sound
    auto * pResourceManager = gameManager.GetManager<ResourceManager>();
    if (!pResourceManager)
    {
        return;
    }
    ResourceId soundId("../../VampireSurvivors/Audio/DoorOpen.mp3");
    auto pBuffer = pResourceManager->GetSoundBuffer(soundId);
    if (pBuffer)
    {
        gameManager.GetManager<AudioManager>()->PlayPooledSound(pBuffer, 20.f, 1.f);
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
            pSpriteComp->GetSprite().setTextureRect(sf::IntRect(0, 0, 32, 32));
            pSpriteComp->GetSprite().setOrigin(16.f, 16.f); // center
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