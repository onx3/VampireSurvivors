#include "AstroidsPrivate.h"
#include "WandComponent.h"
#include "CollisionComponent.h"
#include "DamageComponent.h"
#include "imgui.h"

WandComponent::WandComponent(GameObject * pOwner, GameManager & gameManager)
	: GameComponent(pOwner, gameManager)
	, mHomingShotObjs()
	, mElapsedTime(0.f)
	, mTimeSinceLastShot(0.f)
	, mCooldown(1.5f)
	, mDamagePerShot(100.f)
	, mDamageMult(1.f)
	, mSpeed(150.f)
	, mName("WandComponent")
{

}

//------------------------------------------------------------------------------------------------------------------------

WandComponent::~WandComponent()
{

}

//------------------------------------------------------------------------------------------------------------------------

void WandComponent::Update(float deltaTime)
{
	mTimeSinceLastShot += deltaTime;
	GameObject * pClosestEnemy = nullptr;

	if (mTimeSinceLastShot >= mCooldown)
	{
		pClosestEnemy = FindClosestEnemy();
		if (pClosestEnemy)
		{
			PerformHomingShot(pClosestEnemy);
		}
	}

	mElapsedTime += deltaTime;
	UpdateHomingShots(deltaTime);
}

//------------------------------------------------------------------------------------------------------------------------

void WandComponent::draw(sf::RenderTarget & target, sf::RenderStates states)
{

}

//------------------------------------------------------------------------------------------------------------------------

void WandComponent::DebugImGuiComponentInfo()
{
	ImGui::InputFloat("Raw Damage", &mDamagePerShot);
	ImGui::InputFloat("Damage Multiplier", &mDamageMult);
}

//------------------------------------------------------------------------------------------------------------------------

std::string & WandComponent::GetClassName()
{
	return mName;
}

//------------------------------------------------------------------------------------------------------------------------

void WandComponent::AddDamage(float damage)
{
	mDamagePerShot += damage;
}

//------------------------------------------------------------------------------------------------------------------------

GameObject * WandComponent::FindClosestEnemy()
{
	const auto & myPos = GetGameObject().GetPosition();
	float closestDistSq = std::numeric_limits<float>::max();
	GameObject * pEnemy = nullptr;

	auto & gameManager = GetGameManager();
	auto * pEnemyManager = gameManager.GetManager<EnemyAIManager>();

	if (pEnemyManager)
	{
		auto & enemies = pEnemyManager->GetEnemies();
		for (auto enemy : enemies)
		{
			auto * pObj = gameManager.GetGameObject(enemy);
			if (!pObj || pObj->IsDestroyed())
			{
				continue;
			}
			sf::Vector2f toEnemy = pObj->GetPosition() - myPos;
			float distSq = BD::Dot(toEnemy, toEnemy); // Gives length squared
			if (distSq < closestDistSq)
			{
				closestDistSq = distSq;
				pEnemy = pObj;
			}
		}
	}

	return pEnemy;
}

//------------------------------------------------------------------------------------------------------------------------

void WandComponent::UpdateHomingShots(float deltaTime)
{
	auto & gameManager = GetGameManager();

	for (auto & projectile : mHomingShotObjs)
	{
		GameObject * pHomingShot = gameManager.GetGameObject(projectile.selfHandle);
		GameObject * pTarget = gameManager.GetGameObject(projectile.enemyHandle);

		if (!pHomingShot || pHomingShot->IsDestroyed())
		{
			continue;
		}

		if (!pTarget || pTarget->IsDestroyed())
		{
			pHomingShot->Destroy();
			continue;
		}

		// Homing logic
		sf::Vector2f dir = pTarget->GetPosition() - pHomingShot->GetPosition();
		float length = std::sqrt(dir.x * dir.x + dir.y * dir.y);
		if (length > 0.f)
			dir /= length;

		sf::Vector2f newPosition = pHomingShot->GetPosition() + dir * projectile.speed * deltaTime;
		pHomingShot->SetPosition(newPosition);
	}

	// Clean up
	mHomingShotObjs.erase(
		std::remove_if(mHomingShotObjs.begin(), mHomingShotObjs.end(),
			[&gameManager](const HomingShot & shot)
			{
				GameObject * pHomingShot = gameManager.GetGameObject(shot.selfHandle);
				return !pHomingShot || pHomingShot->IsDestroyed();
			}),
		mHomingShotObjs.end());
}

//------------------------------------------------------------------------------------------------------------------------

void WandComponent::PerformHomingShot(GameObject * pEnemy)
{
	if (!pEnemy)
	{
		return;
	}
	if (mTimeSinceLastShot < mCooldown)
	{
		return;
	}

	mTimeSinceLastShot = 0.f;
	mElapsedTime = 0.f;

	auto & gameManager = GetGameManager();
	auto shotHandle = gameManager.CreateNewGameObject(ETeam::FriendlyFleeting, GetGameObject().GetHandle());
	auto * pShotObj = gameManager.GetGameObject(shotHandle);
	if (!pShotObj)
	{
		return;
	}

	// Sprite Component
	{
		auto pShotSpriteComponent = pShotObj->GetComponent<SpriteComponent>().lock();
		if (pShotSpriteComponent)
		{
			std::string file = "Art/laserGreenShot.png";
			ResourceId resourceId(file);

			auto pTexture = gameManager.GetManager<ResourceManager>()->GetTexture(resourceId);
			if (pTexture)
			{
				pShotSpriteComponent->SetSprite(pTexture, sf::Vector2f(.65f, .65f));
				pShotObj->SetPosition(GetGameObject().GetPosition());
			}
		}
	}

	// Collision Component
	{
		auto pShotCollisionComponent = pShotObj->GetComponent<CollisionComponent>().lock();
		if (!pShotCollisionComponent)
		{
			pShotObj->CreateBoxShapePhysicsBody(&gameManager.GetPhysicsWorld(), pShotObj->GetSize(), true);

			pShotObj->AddComponent(std::make_shared<CollisionComponent>(
				pShotObj,
				gameManager,
				&gameManager.GetPhysicsWorld(),
				pShotObj->GetPhysicsBody(),
				pShotObj->GetSize(),
				true
			));
		}
	}

	// DamageComponent
	{
		auto pShotDamageComponent = pShotObj->GetComponent<DamageComponent>().lock();
		if (!pShotDamageComponent)
		{
			auto pShotDamageComponent = std::make_shared<DamageComponent>(pShotObj, gameManager, mDamagePerShot * mDamageMult);
			pShotObj->AddComponent(pShotDamageComponent);
		}
	}

	HomingShot shot = { shotHandle, pEnemy->GetHandle(), mDamagePerShot * mDamageMult, mSpeed };
	mHomingShotObjs.push_back(shot);
}

//------------------------------------------------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------------------------------------------------