#include "AstroidsPrivate.h"
#include "BoomerangComponent.h"
#include <imgui.h>
#include "PlayerManager.h"
#include "CollisionComponent.h"
#include "DamageComponent.h"
#include "PlayerStatsComponent.h"

namespace
{
	static const float skBoomerangTravelDistance = 8.f;
}

BoomerangComponent::BoomerangComponent(GameObject * pOwner, GameManager & gameManager)
	: GameComponent(pOwner, gameManager)
	, mBoomerangs()
	, mTimeSinceLastThrow(0.f)
	, mCooldown(1.5f)
	, mBoomerangDamagePerThrow(50.f)
	, mBoomerangDamageMult(1.f)
	, mSpeed(20.f)
	, mName("BoomerangComponent")
{

}

//------------------------------------------------------------------------------------------------------------------------

BoomerangComponent::~BoomerangComponent()
{

}

//------------------------------------------------------------------------------------------------------------------------

void BoomerangComponent::Update(float deltaTime)
{
	mTimeSinceLastThrow += deltaTime;
	GameObject * pClosestEnemy = nullptr;

	if (mTimeSinceLastThrow >= mCooldown)
	{
		auto * pPlayerManager = GetGameManager().GetManager<PlayerManager>();
		if (!pPlayerManager)
		{
			return;
		}
		pClosestEnemy = pPlayerManager->FindClosestEnemy();
		if (pClosestEnemy)
		{
			ThrowBoomerang(*pClosestEnemy);
		}
	}

	UpdateBoomerangs(deltaTime);
}

//------------------------------------------------------------------------------------------------------------------------

void BoomerangComponent::DebugImGuiComponentInfo()
{
	ImGui::InputFloat("Raw Damage", &mBoomerangDamagePerThrow);
	ImGui::InputFloat("Damage Multiplier", &mBoomerangDamageMult);
}

//------------------------------------------------------------------------------------------------------------------------

const std::string & BoomerangComponent::GetClassName()
{
	return mName;
}

//------------------------------------------------------------------------------------------------------------------------

void BoomerangComponent::AddDamage(float damage)
{
	mBoomerangDamagePerThrow += damage;
}

//------------------------------------------------------------------------------------------------------------------------

void BoomerangComponent::ThrowBoomerang(GameObject & enemy)
{
	mTimeSinceLastThrow = 0.f;

	auto & gameManager = GetGameManager();
	auto & gameObj = GetGameObject();
	auto boomerangHandle = gameManager.CreateNewGameObject(ETeam::FriendlyPersistant, gameObj.GetHandle());
	auto * pBoomerangObj = gameManager.GetGameObject(boomerangHandle);
	if (!pBoomerangObj)
	{
		return;
	}

	// Sprite Component
	{
		auto pShotSpriteComponent = pBoomerangObj->GetComponent<SpriteComponent>().lock();
		if (pShotSpriteComponent)
		{
			std::string file = "../../VampireSurvivors/Art/Weapons/weapon_throwing_axe.png";
			ResourceId resourceId(file);

			auto pTexture = gameManager.GetManager<ResourceManager>()->GetTexture(resourceId);
			if (pTexture)
			{
				pShotSpriteComponent->SetSprite(pTexture, sf::Vector2f(1.f, 1.f));
				pBoomerangObj->SetPosition(gameObj.GetPosition());
			}
		}
	}

	// Collision Component
	{
        auto pShotCollisionComponent = pBoomerangObj->GetComponent<CollisionComponent>().lock();
        if (!pShotCollisionComponent)
        {
            pBoomerangObj->CreateBoxShapePhysicsBody(
                &gameManager.GetPhysicsWorld(),
                pBoomerangObj->GetSize(),
                true,                  // isDynamic
                true                  // isSensor
            );

            pBoomerangObj->AddComponent(std::make_shared<CollisionComponent>(
                pBoomerangObj,
                gameManager,
                &gameManager.GetPhysicsWorld(),
                pBoomerangObj->GetPhysicsBody(),
                pBoomerangObj->GetSize(),
                true
            ));
        }
	}

	// DamageComponent
	auto pPlayerStatsComp = gameObj.GetComponent<PlayerStatsComponent>().lock();
	float overalDamageMult = 0.0f;
	if (pPlayerStatsComp)
	{
		overalDamageMult = pPlayerStatsComp->GetDamageMult();
	}
	auto pShotDamageComponent = pBoomerangObj->GetComponent<DamageComponent>().lock();
	if (!pShotDamageComponent)
	{
		auto pShotDamageComponent = std::make_shared<DamageComponent>(pBoomerangObj, gameManager, (mBoomerangDamagePerThrow * mBoomerangDamageMult * overalDamageMult));
		pBoomerangObj->AddComponent(pShotDamageComponent);
	}

	auto & enemyPos = enemy.GetPosition();
	auto & myPos = gameObj.GetPosition();
	auto directionVec = enemyPos - myPos;
	float lenSq = BD::GetMagnitudeSquared(directionVec);
	sf::Vector2f direction = directionVec / std::sqrt(lenSq); // Normalize

	float overalRangeMult = pPlayerStatsComp->GetRangeMult();
	float distanceSqr = (skBoomerangTravelDistance * overalRangeMult) * (skBoomerangTravelDistance * overalRangeMult);
    Boomerang boomerang = {};
    boomerang.handle = boomerangHandle;
    boomerang.distanceSqr = distanceSqr;
    boomerang.isReturning = false;
    boomerang.direction = direction;
    boomerang.lastPosition = gameObj.GetPosition();
	mBoomerangs.push_back(boomerang);
}

//------------------------------------------------------------------------------------------------------------------------

void BoomerangComponent::UpdateBoomerangs(float deltaTime)
{
    auto & gameManager = GetGameManager();
    auto * pPlayerManager = gameManager.GetManager<PlayerManager>();
    if (!pPlayerManager || pPlayerManager->GetPlayers().empty())
    {
        return;
    }

    GameObject * pPlayer = gameManager.GetGameObject(pPlayerManager->GetPlayers()[0]);
    if (!pPlayer)
    {
        return;
    }

    for (auto & boomerang : mBoomerangs)
    {
        GameObject * pBoomerang = gameManager.GetGameObject(boomerang.handle);
        if (!pBoomerang || pBoomerang->IsDestroyed())
        {
            continue;
        }

        // Movement
        if (b2Body * pBody = pBoomerang->GetPhysicsBody())
        {
            b2Vec2 velocity(
                boomerang.direction.x * mSpeed / BD::gsPixelsPerMeter,
                boomerang.direction.y * mSpeed / BD::gsPixelsPerMeter
            );
            pBody->SetLinearVelocity(velocity);
        }

        // Rotation
        {
            auto pSpriteComponent = pBoomerang->GetComponent<SpriteComponent>().lock();
            if (pSpriteComponent)
            {
                float currentRotation = pSpriteComponent->GetRotation();
                currentRotation += 360.f * deltaTime;
                if (currentRotation > 360.f) currentRotation -= 360.f;
                pSpriteComponent->SetRotation(currentRotation);
            }
        }

        // Track distance
        const sf::Vector2f currentPosition = pBoomerang->GetPosition();
        sf::Vector2f movement = currentPosition - boomerang.lastPosition;
        float movementSqr = BD::GetMagnitudeSquared(movement);
        boomerang.distanceSqr -= movementSqr;

        boomerang.lastPosition = currentPosition;

        if (!boomerang.isReturning && boomerang.distanceSqr <= 0.0f)
        {
            // Begin return trip using same distance
            boomerang.direction = -boomerang.direction;
            boomerang.isReturning = true;
            boomerang.distanceSqr = skBoomerangTravelDistance * skBoomerangTravelDistance;
        }
        else if (boomerang.isReturning && boomerang.distanceSqr <= 0.0f)
        {
            pBoomerang->Destroy();
        }
    }

    // Clean up destroyed boomerangs
    mBoomerangs.erase(
        std::remove_if(mBoomerangs.begin(), mBoomerangs.end(),
            [&gameManager](Boomerang & boomerang) {
                GameObject * pBoomerang = gameManager.GetGameObject(boomerang.handle);
                return !pBoomerang || pBoomerang->IsDestroyed();
            }),
        mBoomerangs.end());
}

//------------------------------------------------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------------------------------------------------