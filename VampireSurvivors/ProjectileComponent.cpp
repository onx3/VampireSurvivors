#include "AstroidsPrivate.h"
#include "ProjectileComponent.h"
#include <iostream>
#include <cmath>
#include <stack>
#include "GameObject.h"
#include "SpriteComponent.h"
#include "CollisionComponent.h"
#include "HealthComponent.h"
#include "BDConfig.h"
#include "PlayerManager.h"
#include "ResourceManager.h"
#include "CameraManager.h"
#include "imgui.h"
#include "DamageComponent.h"

ProjectileComponent::ProjectileComponent(GameObject * pOwner, GameManager & gameManager)
    : GameComponent(pOwner, gameManager)
    , mProjectiles()
    , mSpeed(50.f)
    , mCooldown(.2f)
    , mTimeSinceLastShot(1.f)
    , mLastUsedProjectile(EProjectileType::GreenLaser)
    , mDamagePerShot(25.f)
    , mName("ProjectileComponent")
{

}

//------------------------------------------------------------------------------------------------------------------------

ProjectileComponent::~ProjectileComponent()
{
}

//------------------------------------------------------------------------------------------------------------------------

std::string ProjectileComponent::GetCorrectProjectileFile()
{
    switch (mLastUsedProjectile)
    {
        case (EProjectileType::GreenLaser):
        {
            mLastUsedProjectile = EProjectileType::RedLaser;
            return "../../VampireSurvivors/Art/laserRed.png";
        }
        default:
        {
            mLastUsedProjectile = EProjectileType::GreenLaser;
            return "../../VampireSurvivors/Art/laserGreen.png";
        }
    }
}

//------------------------------------------------------------------------------------------------------------------------

void ProjectileComponent::Shoot()
{
    GameManager & gameManager = GetGameManager();
    BD::Handle projectileHandle = gameManager.CreateNewGameObject(ETeam::FriendlyFleeting, mOwnerHandle);
    GameObject * pProjectile = gameManager.GetGameObject(projectileHandle);
    GameObject * pOwnerGameObj = gameManager.GetGameObject(mOwnerHandle);

    if (!pProjectile || !pOwnerGameObj)
    {
        return;
    }

    auto pProjectileSpriteComponent = pProjectile->GetComponent<SpriteComponent>().lock();
    if (pProjectileSpriteComponent)
    {
        // Load texture
        auto file = GetCorrectProjectileFile();
        ResourceId resourceId(file);
        auto pTexture = gameManager.GetManager<ResourceManager>()->GetTexture(resourceId);
        pProjectileSpriteComponent->SetSprite(pTexture, sf::Vector2f(.85f, .85f));

        // Get player position
        sf::Vector2f playerPosition = pOwnerGameObj->GetPosition();
        auto crosshairPosition = gameManager.GetManager<CameraManager>()->GetCrosshairPosition();

        sf::Vector2f direction = crosshairPosition - playerPosition;
        float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
        if (length > 0.f)
        {
            direction /= length;
        }

        // Set rotation angle
        float angleRadians = std::atan2(direction.y, direction.x);
        float angleDegrees = angleRadians * (180.f / BD::gsPi);

        // Set sprite visual rotation and position
        pProjectileSpriteComponent->SetRotation(angleDegrees + 90.f); // adjust based on sprite orientation
        pProjectileSpriteComponent->SetPosition(playerPosition);

        // Create physics body (non-sensor, dynamic)
        pProjectile->CreateBoxShapePhysicsBody(
            &gameManager.GetPhysicsWorld(),
            pProjectile->GetSize(),
            true,                   // isDynamic
            true                    // isSensor
        );

        // Optionally rotate the Box2D body
        b2Body * pBody = pProjectile->GetPhysicsBody();
        if (pBody)
        {
            b2Vec2 startPos(playerPosition.x / pProjectile->PIXELS_PER_METER, playerPosition.y / pProjectile->PIXELS_PER_METER);
            float correctedAngle = angleRadians + (BD::gsPi / 2.f);
            pBody->SetTransform(startPos, correctedAngle); // Apply rotation to physics body
        }

        // Add Collision Component
        auto pCollisionComponent = std::make_shared<CollisionComponent>(
            pProjectile,
            gameManager,
            &gameManager.GetPhysicsWorld(),
            pProjectile->GetPhysicsBody(),
            pProjectile->GetSize(),
            true // NOT a sensor if it should hit things
        );
        pProjectile->AddComponent(pCollisionComponent);

        // Add Damage Component
        auto pDamageComponent = std::make_shared<DamageComponent>(pProjectile, gameManager, mDamagePerShot);
        pProjectile->AddComponent(pDamageComponent);

        // Track projectile
        mProjectiles.push_back({ projectileHandle, 5.f, direction });
    }
}

//------------------------------------------------------------------------------------------------------------------------

void ProjectileComponent::Update(float deltaTime)
{
    GameObject * pOwner = GetGameManager().GetGameObject(mOwnerHandle);
    if (!pOwner)
    {
        return;
    }

    mTimeSinceLastShot += deltaTime;

    if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && mTimeSinceLastShot >= mCooldown)
    {
        Shoot();
        mTimeSinceLastShot = 0.0f;
    }

    UpdateProjectiles(deltaTime);
}

//------------------------------------------------------------------------------------------------------------------------

void ProjectileComponent::DebugImGuiComponentInfo()
{
#if IMGUI_ENABLED()
    for (auto & projectile : mProjectiles)
    {
        ImGui::Text("This is a projectile game object");
    }
#endif
}

//------------------------------------------------------------------------------------------------------------------------

std::string & ProjectileComponent::GetClassName()
{
    return mName;
}

//------------------------------------------------------------------------------------------------------------------------

void ProjectileComponent::UpdateProjectiles(float deltaTime)
{
    auto & gameManager = GetGameManager();

    for (auto & projectile : mProjectiles)
    {
        GameObject * pProjectile = gameManager.GetGameObject(projectile.handle);
        if (pProjectile && !pProjectile->IsDestroyed())
        {
            b2Body * pBody = pProjectile->GetPhysicsBody();
            if (pBody)
            {
                // Use physics velocity
                b2Vec2 velocity(
                    projectile.direction.x * mSpeed / pProjectile->PIXELS_PER_METER,
                    projectile.direction.y * mSpeed / pProjectile->PIXELS_PER_METER
                );
                pBody->SetLinearVelocity(velocity);
            }

            projectile.lifespan -= deltaTime;
        }
    }

    mProjectiles.erase(
        std::remove_if(mProjectiles.begin(), mProjectiles.end(),
            [&gameManager](Projectile & proj) {
                GameObject * pProjectile = gameManager.GetGameObject(proj.handle);
                if (pProjectile && !pProjectile->IsDestroyed() && proj.lifespan <= 0.0f)
                {
                    pProjectile->Destroy();
                }
                return !pProjectile || pProjectile->IsDestroyed();
            }),
        mProjectiles.end());
}

//------------------------------------------------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------------------------------------------------