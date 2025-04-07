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

ProjectileComponent::ProjectileComponent(GameObject * pOwner, GameManager & gameManager)
	: GameComponent(pOwner, gameManager)
	, mProjectiles()
	, mSpeed(700.f)
	, mCooldown(.2f)
	, mTimeSinceLastShot(1.f)
	, mLastUsedProjectile(EProjectileType::GreenLaser)
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
			return "Art/laserRed.png";
		}
		default:
		{
			mLastUsedProjectile = EProjectileType::GreenLaser;
			return "Art/laserGreen.png";
		}
	}
}

//------------------------------------------------------------------------------------------------------------------------

void ProjectileComponent::Shoot()
{
	GameManager & gameManager = GetGameManager();
	BD::Handle projectileHandle = gameManager.CreateNewGameObject(ETeam::Friendly, mOwnerHandle);
	GameObject * pProjectile = gameManager.GetGameObject(projectileHandle);
	GameObject * pOwnerGameObj = gameManager.GetGameObject(mOwnerHandle);

	if (!pProjectile || !pOwnerGameObj)
	{
		return;
	}

	auto pProjectileSpriteComponent = pProjectile->GetComponent<SpriteComponent>().lock();
	if (pProjectileSpriteComponent)
	{
		auto file = GetCorrectProjectileFile();
		ResourceId resourceId(file);
		auto pTexture = GetGameManager().GetManager<ResourceManager>()->GetTexture(resourceId);

		pProjectileSpriteComponent->SetSprite(pTexture, sf::Vector2f(1.05f, 1.05f));

		// Get ship's position, size
		sf::Vector2f playerPosition = pOwnerGameObj->GetPosition();
		sf::Vector2f playerSize = pOwnerGameObj->GetSize();

		// Calculate edge offset
		sf::Vector2f offset;
		if (mLastUsedProjectile == EProjectileType::RedLaser)
		{
			offset = sf::Vector2f(playerSize.y / 2.f, 0);
		}
		else
		{
			offset = sf::Vector2f(-playerSize.y / 2.f, 0);
		}

		sf::Vector2f spawnPosition = playerPosition + offset;

		auto crosshairPosition = GetGameObject().GetGameManager().GetManager<CameraManager>()->GetCrosshairPosition();
		sf::Vector2f direction = crosshairPosition - spawnPosition;
		float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
		if (length != 0)
		{
			direction /= length; // Normalize
		}

		// Calculate angle in degrees
		float angleDegrees = std::atan2(direction.y, direction.x) * (180.f / 3.14159265f);

		// Set projectile position and rotation
		pProjectileSpriteComponent->SetPosition(spawnPosition);
		pProjectileSpriteComponent->SetRotation(angleDegrees + 90.f); // Adjust rotation for sprite alignment

		// Add collision
		pProjectile->CreatePhysicsBody(&pOwnerGameObj->GetGameManager().GetPhysicsWorld(), pProjectile->GetSize(), true);
		auto pCollisionComponent = std::make_shared<CollisionComponent>(
			pProjectile,
			gameManager,
			&pOwnerGameObj->GetGameManager().GetPhysicsWorld(),
			pProjectile->GetPhysicsBody(),
            pProjectile->GetSize(), 
            true
        );
		pProjectile->AddComponent(pCollisionComponent);
		mProjectiles.push_back({ projectileHandle, 3.f, 15, direction });
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
			sf::Vector2f currentPosition = pProjectile->GetPosition();
			sf::Vector2f newPosition = currentPosition + (projectile.direction * mSpeed * deltaTime);
			pProjectile->SetPosition(newPosition);

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