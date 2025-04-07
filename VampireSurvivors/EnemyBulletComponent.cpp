#include "AstroidsPrivate.h"
#include "EnemyBulletComponent.h"
#include "ProjectileComponent.h"
#include "CameraManager.h"
#include "CollisionComponent.h"

namespace
{
	static const float skBulletScale = 1.05f;
	static const float skBulletLifeTime = 3.f;
	static const int skBulletDamage = 15;

}

EnemyBulletComponent::EnemyBulletComponent(GameObject * pOwner, GameManager & gameManager)
	: GameComponent(pOwner, gameManager)
	, mBullets()
	, mSpeed(500.f)
	, mCooldown(.1f)
	, mTimeSinceLastShot(1.f)
	, mLastUsedProjectile(EProjectileType::GreenLaser)
	, mName("EnemyBulletComponent")
{

}

//------------------------------------------------------------------------------------------------------------------------

EnemyBulletComponent::~EnemyBulletComponent()
{

}

//------------------------------------------------------------------------------------------------------------------------

std::string EnemyBulletComponent::GetCorrectProjectileFile()
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

void EnemyBulletComponent::Shoot()
{
	GameManager & gameManager = GetGameManager();
	BD::Handle bulletHandle = gameManager.CreateNewGameObject(ETeam::Enemy, mOwnerHandle);
	GameObject * pBullet = gameManager.GetGameObject(bulletHandle);
	GameObject * pOwnerGameObj = gameManager.GetGameObject(mOwnerHandle);

	if (!pBullet || !pOwnerGameObj)
	{
		return;
	}

	auto pBulletSpriteComp = pBullet->GetComponent<SpriteComponent>().lock();
	if (pBulletSpriteComp)
	{
		// Sprite
		auto filePath = GetCorrectProjectileFile();
		ResourceId resourceId(filePath);
		auto pTexture = GetGameManager().GetManager<ResourceManager>()->GetTexture(resourceId);
		
		pBulletSpriteComp->SetSprite(pTexture, sf::Vector2f(skBulletScale, skBulletScale));

		// Get spawn Position
		sf::Vector2f spawnPosition = pOwnerGameObj->GetPosition();
		float rotation = pOwnerGameObj->GetRotationRadians();
		sf::Vector2f directionVec = { std::sin(rotation), -std::cos(rotation) };

		float angleDegrees = std::atan2(directionVec.y, directionVec.x) * (180.f / 3.14159265f);

		// Set projectile position and rotation
		pBulletSpriteComp->SetPosition(spawnPosition);
		pBulletSpriteComp->SetRotation(angleDegrees + 90.f);

		pBullet->CreatePhysicsBody(&pOwnerGameObj->GetGameManager().GetPhysicsWorld(), pBullet->GetSize(), true);
		auto pCollisionComponent = std::make_shared<CollisionComponent>(
			pBullet,
			gameManager,
			&pOwnerGameObj->GetGameManager().GetPhysicsWorld(),
			pBullet->GetPhysicsBody(),
			pBullet->GetSize(),
			true
		);
		pBullet->AddComponent(pCollisionComponent);
		mBullets.push_back({ bulletHandle, skBulletLifeTime, skBulletDamage, directionVec });
	}
}

//------------------------------------------------------------------------------------------------------------------------

void EnemyBulletComponent::Update(float deltaTime)
{
	GameObject * pOwner = GetGameManager().GetGameObject(mOwnerHandle);
	if (!pOwner)
	{
		return;
	}

	mTimeSinceLastShot += deltaTime;
	if (mTimeSinceLastShot >= mCooldown)
	{
		Shoot();
		mTimeSinceLastShot = 0.0f;
	}

	UpdateProjectiles(deltaTime);
}

//------------------------------------------------------------------------------------------------------------------------

void EnemyBulletComponent::DebugImGuiComponentInfo()
{
}

//------------------------------------------------------------------------------------------------------------------------

std::string & EnemyBulletComponent::GetClassName()
{
	return mName;
}

//------------------------------------------------------------------------------------------------------------------------

void EnemyBulletComponent::UpdateProjectiles(float deltaTime)
{
	auto & gameManager = GetGameManager();

	for (auto & projectile : mBullets)
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

	mBullets.erase(
		std::remove_if(mBullets.begin(), mBullets.end(),
			[&gameManager](Projectile & proj) {
				GameObject * pProjectile = gameManager.GetGameObject(proj.handle);
				if (pProjectile && !pProjectile->IsDestroyed() && proj.lifespan <= 0.0f)
				{
					pProjectile->Destroy();
				}
				return !pProjectile || pProjectile->IsDestroyed();
			}),
		mBullets.end());
}

//------------------------------------------------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------------------------------------------------