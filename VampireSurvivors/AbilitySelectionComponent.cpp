#include "AstroidsPrivate.h"
#include "AbilitySelectionComponent.h"
#include "PlayerManager.h"
#include "AbilityUIManager.h"

AbilitySelectionComponent::AbilitySelectionComponent(GameObject * pOwner, GameManager & gameManager)
    : GameComponent(pOwner, gameManager)
    , mInteractionRange(50.f)
    , mIsPlayerInRange(false)
    , mName("AbilitySelectionComponent")
{
    if (!mFont.loadFromFile("../../VampireSurvivors/Art/font.ttf"))
    {
        std::cerr << "Failed to load font\n";
    }

    mInteractionText.setFont(mFont);
    mInteractionText.setCharacterSize(15);
    mInteractionText.setFillColor(sf::Color::White);
    char buffer[64];
    std::snprintf(buffer, sizeof(buffer), "Press F to open ability selector");
    mInteractionText.setString(buffer);
}

//------------------------------------------------------------------------------------------------------------------------

AbilitySelectionComponent::~AbilitySelectionComponent()
{
}

//------------------------------------------------------------------------------------------------------------------------

void AbilitySelectionComponent::Update(float deltaTime)
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

    if (mIsPlayerInRange)
    {
        mInteractionText.setPosition(doorObj.GetPosition().x, doorObj.GetPosition().y - 40.f);

        if (BD::IsKeyJustPressed(sf::Keyboard::F))
        {
            gameManager.SetGameState(EGameState::AbilitySelect);
        }
    }
}

//------------------------------------------------------------------------------------------------------------------------

void AbilitySelectionComponent::draw(sf::RenderTarget & target, sf::RenderStates states)
{
    if (mIsPlayerInRange)
    {
        target.draw(mInteractionText);
    }
}

//------------------------------------------------------------------------------------------------------------------------

void AbilitySelectionComponent::DebugImGuiComponentInfo()
{
}

//------------------------------------------------------------------------------------------------------------------------

std::string & AbilitySelectionComponent::GetClassName()
{
    return mName;
}

//------------------------------------------------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------------------------------------------------