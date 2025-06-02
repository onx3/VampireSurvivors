#include "AstroidsPrivate.h"
#include "FirePotComponent.h"
#include "DamageZoneComponent.h"
#include "PlayerManager.h"
#include "PlayerStatsComponent.h"
#include "SpriteAnimationComponent.h"
#include "DamageComponent.h"
#include "CollisionComponent.h"
#include "LightComponent.h"

namespace 
{
    const float skCoolDown = 4.f;
    const float skLifeTime = 5.f;
}

//------------------------------------------------------------------------------------------------------------------------

FirePotComponent::FirePotComponent(GameObject * pOwner, GameManager & gameManager)
    : GameComponent(pOwner, gameManager)
    , mFirePots()
    , mGhostTrails()
    , mTimeSinceLastCast(0)
    , mCooldown(skCoolDown)
    , mDamagePerSecond(50.f)
    , mDamageMult(1.f)
    , mSpeed(200.f)
    , mName("FirePotComponent")
{

}

//------------------------------------------------------------------------------------------------------------------------

void FirePotComponent::Update(float deltaTime)
{
    mTimeSinceLastCast += deltaTime;
    GameObject * pClosestEnemy = nullptr;

    if (mTimeSinceLastCast >= mCooldown)
    {
        auto * pPlayerManager = GetGameManager().GetManager<PlayerManager>();
        if (!pPlayerManager)
        {
            return;
        }
        pClosestEnemy = pPlayerManager->FindClosestEnemy();
        if (pClosestEnemy)
        {
            CastFirePot(*pClosestEnemy);
        }
    }
    UpdateFirePots(deltaTime);
}

//------------------------------------------------------------------------------------------------------------------------

void FirePotComponent::DebugImGuiComponentInfo()
{

}

//------------------------------------------------------------------------------------------------------------------------

std::string & FirePotComponent::GetClassName()
{
    return mName;
}

//------------------------------------------------------------------------------------------------------------------------

void FirePotComponent::CastFirePot(GameObject & enemy)
{
    mTimeSinceLastCast = 0.f;

    auto & gameManager = GetGameManager();
    auto & gameObj = GetGameObject();
    auto potHandle = gameManager.CreateNewGameObject(ETeam::FriendlyPersistant, gameObj.GetHandle());
    auto * pPot = gameManager.GetGameObject(potHandle);
    if (!pPot)
    {
        return;
    }

    // Sprite Component
    {
        auto pPotSpriteComponent = pPot->GetComponent<SpriteComponent>().lock();
        if (pPotSpriteComponent)
        {
            std::string file = "../../VampireSurvivors/Art/Fire/bomb_f1.png";
            ResourceId resourceId(file);

            auto pTexture = gameManager.GetManager<ResourceManager>()->GetTexture(resourceId);
            if (pTexture)
            {
                pPotSpriteComponent->SetSprite(pTexture, sf::Vector2f(1.f, 1.f));
            }
        }
    }

    // Light Component
    {
        auto pLightComponent = pPot->GetComponent<LightComponent>().lock();
        if (!pLightComponent)
        {
            pLightComponent = std::make_shared<LightComponent>(pPot, gameManager, 10.f, sf::Color::White);
            pPot->AddComponent(pLightComponent);
        }
    }

    const sf::Vector2f & playerPos = GetGameObject().GetPosition();
    const sf::Vector2f & enemyPos = enemy.GetPosition();
    sf::Vector2f midpoint = (playerPos + enemyPos) * 0.5f;

    sf::View view = gameManager.GetWindow().getView();
    sf::Vector2f viewCenter = view.getCenter();
    sf::Vector2f viewSize = view.getSize();

    float aboveScreenOffset = 50.f;

    float viewTop = viewCenter.y - viewSize.y / 2.f;

    pPot->SetPosition(sf::Vector2f(midpoint.x, viewTop - aboveScreenOffset));

    FirePot firePot = { potHandle, midpoint, false, skLifeTime};
    mFirePots.push_back(firePot);
}

//------------------------------------------------------------------------------------------------------------------------

void FirePotComponent::CreateSpriteAnimationComponent(GameObject & pot)
{
    GameManager & gameManager = GetGameManager();
    auto pAnimComponent = pot.GetComponent<SpriteAnimationComponent>().lock();
    if (!pAnimComponent)
    {
        pAnimComponent = std::make_shared<SpriteAnimationComponent>(&pot, gameManager);
        pot.AddComponent(pAnimComponent);

        // Create Start animation (Top row: y = 0)
        Animation startAnimation;
        startAnimation.frames = {
            sf::IntRect(0,   0, 24, 32), // Frame 0
            sf::IntRect(24,  0, 24, 32), // Frame 1
            sf::IntRect(48,  0, 24, 32), // Frame 2
            sf::IntRect(72,  0, 24, 32)  // Frame 3
        };
        startAnimation.frameTime = 0.2f;

        // Create Loop animation (Second row: y = 32)
        Animation loopAnimation;
        loopAnimation.frames = {
            sf::IntRect(0,   32, 24, 32), // Frame 0
            sf::IntRect(24,  32, 24, 32), // Frame 1
            sf::IntRect(48,  32, 24, 32), // Frame 2
            sf::IntRect(72,  32, 24, 32),  // Frame 3
            sf::IntRect(96,  32, 24, 32),  // Frame 4
            sf::IntRect(120,  32, 24, 32),  // Frame 5
            sf::IntRect(144,  32, 24, 32),  // Frame 6
            sf::IntRect(168,  32, 24, 32),  // Frame 7
        };
        loopAnimation.frameTime = 0.15f;

        // Create End animation (Third row: y = 64)
        Animation endAnimation;
        endAnimation.frames = {
            sf::IntRect(0,   64, 24, 32), // Frame 0
            sf::IntRect(24,  64, 24, 32), // Frame 1
            sf::IntRect(48,  64, 24, 32), // Frame 2
            sf::IntRect(72,  64, 24, 32),  // Frame 3
            sf::IntRect(96,  64, 24, 32)  // Frame 4
        };
        endAnimation.frameTime = 0.2f;

        // Register the animations
        pAnimComponent->AddAnimation(EAnimationState::Start, startAnimation);
        pAnimComponent->AddAnimation(EAnimationState::Loop, loopAnimation);
        pAnimComponent->AddAnimation(EAnimationState::End, endAnimation);

        // Start playing the Idle animation
        pAnimComponent->PlayAnimation(EAnimationState::Loop);
    }
}

//------------------------------------------------------------------------------------------------------------------------

void FirePotComponent::UpdateFirePots(float deltaTime)
{
    auto & gameManager = GetGameManager();

    for (int ii = int(mFirePots.size()) - 1; ii >= 0; --ii)
    {
        auto & pot = mFirePots[ii];
        GameObject * pPot = gameManager.GetGameObject(pot.firePotHandle);

        if (!pPot || pPot->IsDestroyed())
        {
            mFirePots.erase(mFirePots.begin() + ii);
            continue;
        }

        const sf::Vector2f & currentPos = pPot->GetPosition();
        sf::Vector2f toTarget = pot.position - currentPos;
        float dist = std::sqrt(toTarget.x * toTarget.x + toTarget.y * toTarget.y);

        if (!pot.hasLanded)
        {
            if (dist > 1.0f)
            {
                sf::Vector2f dir = toTarget / dist;
                sf::Vector2f newPos = currentPos + dir * mSpeed * deltaTime;
                pPot->SetPosition(newPos);

                // Rotate while falling
                {
                    auto pSpriteComponent = pPot->GetComponent<SpriteComponent>().lock();
                    if (pSpriteComponent)
                    {
                        float currentRotation = pSpriteComponent->GetRotation();
                        currentRotation += 360.f * deltaTime;
                        if (currentRotation > 360.f)
                        {
                            currentRotation -= 360.f;
                        }
                        pSpriteComponent->SetRotation(currentRotation);
                    }
                }
            }
            else
            {
                pot.hasLanded = true;
                pPot->SetPosition(pot.position); // snap to exact position

                // Collision Component
                if (!pPot->GetComponent<CollisionComponent>().lock())
                {
                    pPot->CreateCircleShapePhysicsBody(&gameManager.GetPhysicsWorld(), 32.f, true, false);

                    pPot->AddComponent(std::make_shared<CollisionComponent>(
                        pPot,
                        gameManager,
                        &gameManager.GetPhysicsWorld(),
                        pPot->GetPhysicsBody(),
                        sf::Vector2f(64.f, 64.f),
                        true // isSensor
                    ));
                }
                
                // Sprite Animation Component
                CreateSpriteAnimationComponent(*pPot);

                // Swap sprite to fire zone
                auto pSprite = pPot->GetComponent<SpriteComponent>().lock();
                if (pSprite)
                {
                    pSprite->SetRotation(0.f); // Spawn fire upright
                    auto * pResMgr = GetGameManager().GetManager<ResourceManager>();
                    if (pResMgr)
                    {
                        ResourceId resId = ResourceId("../../VampireSurvivors/Art/Fire/orange/FireOrange.png");
                        auto pFireTex = pResMgr->GetTexture(resId);
                        if (pFireTex)
                        {
                            pSprite->SetSprite(pFireTex, sf::Vector2f(2.f, 2.f));
                            pSprite->GetSprite().setColor(sf::Color(255, 140, 0, 180)); // orange glow
                        }
                    }
                }

                // Light Component
                {
                    auto pLightComponent = pPot->GetComponent<LightComponent>().lock();
                    if (pLightComponent)
                    {
                        pLightComponent->SetColor(sf::Color(255, 140, 0, 180));
                        pLightComponent->SetRadius(50.f);
                    }
                }

                // Add AoE damage component
                float damagePerSecond = 30.f;
                float duration = pot.timeLeft;
                pPot->AddComponent(std::make_shared<DamageZoneComponent>(
                    pPot, gameManager, damagePerSecond, duration));
            }
        }
        else
        {
            pot.timeLeft -= deltaTime;
            if (pot.timeLeft <= 0.f)
            {
                pPot->Destroy();
                mFirePots.erase(mFirePots.begin() + ii);
                continue;
            }
        }
    }
}

//------------------------------------------------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------------------------------------------------