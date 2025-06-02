#include "AstroidsPrivate.h"
#include "ThrowingKnife.h"
#include "CameraManager.h"
#include "CollisionComponent.h"
#include "DamageComponent.h"
#include "PlayerStatsComponent.h"
#include <imgui.h>

namespace
{
    static const float sThrowingKnifeLifeTime = 3.0f;
}

ThrowingKnifeComponent::ThrowingKnifeComponent(GameObject * pOwner, GameManager & gameManager)
    : GameComponent(pOwner, gameManager)
    , mThrowingKnives()
    , mTimeSinceLastShot(0.f)
    , mCooldown(1.2f)
    , mThrowingKnifeDamagePerShot(75.f)
    , mThrowingKnifeDamageMult(1.f)
    , mSpeed(150.f)
    , mName("ThrowingKnifeComponent")
{

}

//------------------------------------------------------------------------------------------------------------------------

ThrowingKnifeComponent::~ThrowingKnifeComponent()
{

}

//------------------------------------------------------------------------------------------------------------------------

void ThrowingKnifeComponent::Update(float deltaTime)
{
    mTimeSinceLastShot += deltaTime;
    GameObject * pClosestEnemy = nullptr;

    if (mTimeSinceLastShot >= mCooldown)
    {
        ThrowKnife();
    }

    UpdateThrowingKnives(deltaTime);
}

//------------------------------------------------------------------------------------------------------------------------

void ThrowingKnifeComponent::DebugImGuiComponentInfo()
{
    ImGui::InputFloat("Raw Damage", &mThrowingKnifeDamagePerShot);
    ImGui::InputFloat("Damage Multiplier", &mThrowingKnifeDamageMult);
}

//------------------------------------------------------------------------------------------------------------------------

std::string & ThrowingKnifeComponent::GetClassName()
{
    return mName;
}

//------------------------------------------------------------------------------------------------------------------------

void ThrowingKnifeComponent::AddDamage(float damage)
{
    mThrowingKnifeDamagePerShot += damage;
}

//------------------------------------------------------------------------------------------------------------------------

void ThrowingKnifeComponent::ThrowKnife()
{
    mTimeSinceLastShot = 0.f;

    auto & gameManager = GetGameManager();
    auto & gameObj = GetGameObject();

    auto knifeHandle = gameManager.CreateNewGameObject(ETeam::FriendlyPersistant, gameObj.GetHandle());
    auto * pKnifeObj = gameManager.GetGameObject(knifeHandle);
    if (!pKnifeObj)
    {
        return;
    }

    sf::Vector2f direction;
    // Sprite Component
    {
        auto pKnifeSpriteComponent = pKnifeObj->GetComponent<SpriteComponent>().lock();
        if (pKnifeSpriteComponent)
        {
            std::string file = "../../VampireSurvivors/Art/Weapons/weapon_knife.png";
            ResourceId resourceId(file);

            auto pTexture = gameManager.GetManager<ResourceManager>()->GetTexture(resourceId);
            if (pTexture)
            {
                pKnifeSpriteComponent->SetSprite(pTexture, sf::Vector2f(1.f, 1.f));

                auto * pCameraManager = GetGameManager().GetManager<CameraManager>();
                auto & crosshairPosition = pCameraManager->GetCrosshairPosition();
                direction = crosshairPosition - gameObj.GetPosition();
                float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
                if (length != 0)
                {
                    direction /= length; // Normalize
                }

                // Calculate angle in degrees
                float angleDegrees = std::atan2(direction.y, direction.x) * (180.f / BD::gsPi);

                // Set projectile position and rotation
                pKnifeSpriteComponent->SetPosition(gameObj.GetPosition());
                pKnifeSpriteComponent->SetRotation(angleDegrees + 90.f); // Adjust rotation for sprite alignment
            }
        }
    }
    // Collision Component
    {
        auto pShotCollisionComponent = pKnifeObj->GetComponent<CollisionComponent>().lock();
        if (!pShotCollisionComponent)
        {
            pKnifeObj->CreateBoxShapePhysicsBody(
                &gameManager.GetPhysicsWorld(),
                pKnifeObj->GetSize(),
                true,                   // isDynamic
                true                   // isSensor
            );

            pKnifeObj->AddComponent(std::make_shared<CollisionComponent>(
                pKnifeObj,
                gameManager,
                &gameManager.GetPhysicsWorld(),
                pKnifeObj->GetPhysicsBody(),
                pKnifeObj->GetSize(),
                true
            ));
        }
    }
    // DamageComponent
    {
        auto pShotDamageComponent = pKnifeObj->GetComponent<DamageComponent>().lock();
        if (!pShotDamageComponent)
        {
            auto pPlayerStatsComp = gameObj.GetComponent<PlayerStatsComponent>().lock();
            float overalDamageMult = 0.0f;
            if (pPlayerStatsComp)
            {
                overalDamageMult = pPlayerStatsComp->GetDamageMult();
            }
            auto pShotDamageComponent = std::make_shared<DamageComponent>(pKnifeObj, gameManager, (mThrowingKnifeDamagePerShot * mThrowingKnifeDamageMult * overalDamageMult));
            pKnifeObj->AddComponent(pShotDamageComponent);
        }
    }

    ThrowingKnife knife = { knifeHandle, sThrowingKnifeLifeTime, direction };
    mThrowingKnives.push_back(knife);
}

//------------------------------------------------------------------------------------------------------------------------

void ThrowingKnifeComponent::UpdateThrowingKnives(float deltaTime)
{
    auto & gameManager = GetGameManager();

    for (auto & projectile : mThrowingKnives)
    {
        GameObject * pProjectile = gameManager.GetGameObject(projectile.handle);
        if (pProjectile && !pProjectile->IsDestroyed())
        {
            const sf::Vector2f & currentPosition = pProjectile->GetPosition();
            sf::Vector2f newPosition = currentPosition + (projectile.direction * mSpeed * deltaTime);
            pProjectile->SetPosition(newPosition);

            projectile.lifespan -= deltaTime;
        }
    }

    mThrowingKnives.erase(
        std::remove_if(mThrowingKnives.begin(), mThrowingKnives.end(),
            [&gameManager](ThrowingKnife & knife) {
                GameObject * pThrowingKnife = gameManager.GetGameObject(knife.handle);
                if (pThrowingKnife && !pThrowingKnife->IsDestroyed() && knife.lifespan <= 0.0f)
                {
                    pThrowingKnife->Destroy();
                }
                return !pThrowingKnife || pThrowingKnife->IsDestroyed();
            }),
        mThrowingKnives.end());
}

//------------------------------------------------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------------------------------------------------