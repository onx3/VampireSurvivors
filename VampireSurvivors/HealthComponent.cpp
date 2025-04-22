#include "AstroidsPrivate.h"
#include "HealthComponent.h"
#include <iostream>
#include "PlayerManager.h"
#include "ExplosionComponent.h"
#include "SpriteComponent.h"
#include <functional>
#include <cmath>
#include "imgui.h"

HealthComponent::HealthComponent(GameObject * pOwner, GameManager & gameManager, float initialHealth, float maxHealth, int lifeCount, int maxLives, float hitCooldown)
    : GameComponent(pOwner, gameManager)
    , mHealth(initialHealth)
    , mMaxHealth(maxHealth)
    , mLifeCount(lifeCount)
    , mMaxLives(maxLives)
    , mHitCooldown(hitCooldown)
    , mTimeSinceLastHit(0.f)
    , mName("HealthComponent")
{
}

//------------------------------------------------------------------------------------------------------------------------

float HealthComponent::GetHealth() const
{
    return mHealth;
}

//------------------------------------------------------------------------------------------------------------------------

void HealthComponent::AddHealth(float amount)
{
    mHealth += amount;
    if (mHealth > mMaxHealth)
    { 
        mHealth = mMaxHealth;
    }
}

//------------------------------------------------------------------------------------------------------------------------

void HealthComponent::LoseHealth(float amount)
{
    if (mTimeSinceLastHit >= mHitCooldown)
    {
        mHealth -= amount;
        if (mHealth <= 0)
        {
            mHealth = 0;
        }

        // Damage numbers
        GameManager & gameManager = GetGameManager();
        auto pUIManager = gameManager.GetManager<UIManager>();
        if (pUIManager)
        {
            auto & gameObj = GetGameObject();
            sf::Color color = sf::Color::White;
            if (gameObj.GetTeam() == ETeam::Player)
            {
                color = sf::Color::Red;
            }
            pUIManager->AddDamageNumber(GetGameObject().GetPosition(), amount, color);
        }
        mTimeSinceLastHit = 0.0f;
    }
}

//------------------------------------------------------------------------------------------------------------------------

int HealthComponent::GetLives() const
{
    return mLifeCount;
}

//------------------------------------------------------------------------------------------------------------------------

void HealthComponent::AddLife(int amount)
{
    mLifeCount = std::min(mLifeCount + amount, mMaxLives);
}

//------------------------------------------------------------------------------------------------------------------------

float HealthComponent::GetMaxHealth() const
{
    return mMaxHealth;
}

//------------------------------------------------------------------------------------------------------------------------

void HealthComponent::AddMaxHealth(float amount)
{
    mMaxHealth += amount;
}

//------------------------------------------------------------------------------------------------------------------------

void HealthComponent::SetDeathCallBack(std::function<void()> callback)
{
    mDeathCallback = callback;
}

//------------------------------------------------------------------------------------------------------------------------

void HealthComponent::SetLifeLostCallback(std::function<void()> callback)
{
    mLifeLostCallback = callback;
}

//------------------------------------------------------------------------------------------------------------------------

void HealthComponent::LoseLife()
{
    if (mLifeCount == 1)
    {
        --mLifeCount;
        if (mDeathCallback)
        {
            mDeathCallback();
        }
    }
    else if (mLifeCount > 0)
    {
        --mLifeCount;
        if (mLifeLostCallback)
        {
            mLifeLostCallback();
        }
    }
}

//------------------------------------------------------------------------------------------------------------------------

void HealthComponent::Update(float deltaTime)
{
    GameObject * pOwner = GetGameManager().GetGameObject(mOwnerHandle);
    if (!pOwner)
    {
        return;
    }

    // Increment time since last hit
    mTimeSinceLastHit += deltaTime;

    if (mHealth <= 0)
    {
        if (mLifeCount <= 1)
        {
            pOwner->Deactivate();
        }
        else
        {
            mHealth = mMaxHealth;
        }
        LoseLife();
    }

    if (pOwner->IsDestroyed())
    {
        return;
    }

    // Handle invincibility flickering effect
    auto pSpriteComp = pOwner->GetComponent<SpriteComponent>().lock();
    if (pSpriteComp)
    {
        sf::Color redTint(255, 0, 0, 255);
        pSpriteComp->GetSprite().setColor(redTint);

        if (mTimeSinceLastHit < mHitCooldown)
        {
            float t = std::sin(mTimeSinceLastHit * 10.0f) * 0.5f + 0.5f; // oscillates between 0 and 1
            sf::Uint8 r = static_cast<sf::Uint8>(255);
            sf::Uint8 g = static_cast<sf::Uint8>(255 * (1.0f - t));
            sf::Uint8 b = static_cast<sf::Uint8>(255 * (1.0f - t));
            sf::Color flashColor(r, g, b, 255);
            pSpriteComp->GetSprite().setColor(flashColor);
        }
        else
        {
            pSpriteComp->GetSprite().setColor(sf::Color::White);
        }
    }
}

//------------------------------------------------------------------------------------------------------------------------

void HealthComponent::DebugImGuiComponentInfo()
{
    ImGui::Text("Current amount of lives: %i", mLifeCount);
    ImGui::Text("Max Lives: %i", mMaxLives);
    ImGui::Text("Current Health: %.3f", mHealth);
    ImGui::Text("Max Health: %.3f", mMaxHealth);
}

//------------------------------------------------------------------------------------------------------------------------

std::string & HealthComponent::GetClassName()
{
    return mName;
}

//------------------------------------------------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------------------------------------------------
