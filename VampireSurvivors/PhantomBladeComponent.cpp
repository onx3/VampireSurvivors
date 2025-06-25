#include "AstroidsPrivate.h"
#include "PhantomBladeComponent.h"
#include "PlayerManager.h"
#include "CollisionComponent.h"
#include "DamageComponent.h"
#include "PlayerStatsComponent.h"

namespace
{
	static const float skPhantomBladeMaxDistance = 150.f;
	static const float skPhantomBladeLifeTime = 4.f;
	static const float skGhostLifeTime = .2f;
}

PhantomBladeComponent::PhantomBladeComponent(GameObject * pOwner, GameManager & gameManager)
	: GameComponent(pOwner, gameManager)
	, mPhantomBlades()
	, mGhostTrails()
	, mTimeSinceLastCast(10.f)
	, mCooldown(skPhantomBladeLifeTime * 2) // Keep uptime at 50%
	, mDamagePerSlash(200.f)
	, mDamageMult(1.f)
	, mSpeed(25.f)
	, mName("PhantomBladeComponent")
{

}

//------------------------------------------------------------------------------------------------------------------------

PhantomBladeComponent::~PhantomBladeComponent()
{

}

//------------------------------------------------------------------------------------------------------------------------

void PhantomBladeComponent::Update(float deltaTime)
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
			auto & gameObj = GetGameObject();
			auto pPlayerStatsComp = gameObj.GetComponent<PlayerStatsComponent>().lock();
			if (pPlayerStatsComp)
			{
				float playerRangeMult = pPlayerStatsComp->GetRangeMult();
				float maxDistanceSqr = (skPhantomBladeMaxDistance * playerRangeMult) * (skPhantomBladeMaxDistance * playerRangeMult);
				float distanceSqr = BD::GetMagnitudeSquared(gameObj.GetPosition(), pClosestEnemy->GetPosition());

				if (distanceSqr <= maxDistanceSqr)
				{
					CastPhantomBlade(*pClosestEnemy);
				}
			}			
		}
	}
	UpdatePhantomBlades(deltaTime);
}

//------------------------------------------------------------------------------------------------------------------------

void PhantomBladeComponent::draw(sf::RenderTarget & target, sf::RenderStates states)
{
	DrawGhostTrails(GetGameManager().GetWindow());
}

//------------------------------------------------------------------------------------------------------------------------

void PhantomBladeComponent::DebugImGuiComponentInfo()
{

}

//------------------------------------------------------------------------------------------------------------------------

const std::string & PhantomBladeComponent::GetClassName()
{
	return mName;
}

//------------------------------------------------------------------------------------------------------------------------

void PhantomBladeComponent::AddDamage(float damage)
{
	mDamagePerSlash += damage;
}

//------------------------------------------------------------------------------------------------------------------------

void PhantomBladeComponent::CastPhantomBlade(GameObject & enemy)
{
	mTimeSinceLastCast = 0.f;

	auto & gameManager = GetGameManager();
	auto & gameObj = GetGameObject();
	auto phantomHandle = gameManager.CreateNewGameObject(ETeam::FriendlyPersistant, gameObj.GetHandle());
	auto * pPhantomeBlade = gameManager.GetGameObject(phantomHandle);
	if (!pPhantomeBlade)
	{
		return;
	}

	// Sprite Component
	{
		auto pShotSpriteComponent = pPhantomeBlade->GetComponent<SpriteComponent>().lock();
		if (pShotSpriteComponent)
		{
			std::string file = "../../VampireSurvivors/Art/Weapons/weapon_knight_sword.png";
			ResourceId resourceId(file);

			auto pTexture = gameManager.GetManager<ResourceManager>()->GetTexture(resourceId);
			if (pTexture)
			{
				pShotSpriteComponent->SetSprite(pTexture, sf::Vector2f(1.f, 1.f));
				pPhantomeBlade->SetPosition(gameObj.GetPosition());
			}
		}
	}

	// Collision Component
	{
		auto pShotCollisionComponent = pPhantomeBlade->GetComponent<CollisionComponent>().lock();
		if (!pShotCollisionComponent)
		{
            pPhantomeBlade->CreateBoxShapePhysicsBody(
                &gameManager.GetPhysicsWorld(),
                pPhantomeBlade->GetSize(),
                true,                   // isDynamic
                true                   // isSensor
            );

			pPhantomeBlade->AddComponent(std::make_shared<CollisionComponent>(
				pPhantomeBlade,
				gameManager,
				&gameManager.GetPhysicsWorld(),
				pPhantomeBlade->GetPhysicsBody(),
				pPhantomeBlade->GetSize(),
				true
			));
		}
	}

	// DamageComponent
	auto pPlayerStatsComp = gameObj.GetComponent<PlayerStatsComponent>().lock();
	float overalDamageMult = 1.0f;
	if (pPlayerStatsComp)
	{
		overalDamageMult = pPlayerStatsComp->GetDamageMult();
	}
	auto pShotDamageComponent = pPhantomeBlade->GetComponent<DamageComponent>().lock();
	if (!pShotDamageComponent)
	{
		auto pShotDamageComponent = std::make_shared<DamageComponent>(pPhantomeBlade, gameManager, (mDamagePerSlash * mDamageMult * overalDamageMult));
		pPhantomeBlade->AddComponent(pShotDamageComponent);
	}

	PhantomBlade phantomBlade = { phantomHandle, enemy.GetHandle(), skPhantomBladeLifeTime };
	mPhantomBlades.push_back(phantomBlade);
}

//------------------------------------------------------------------------------------------------------------------------

void PhantomBladeComponent::UpdatePhantomBlades(float deltaTime)
{
    auto & gameManager = GetGameManager();
    auto * pPlayerManager = gameManager.GetManager<PlayerManager>();
    if (!pPlayerManager)
    {
        return;
    }

    for (int i = static_cast<int>(mPhantomBlades.size()) - 1; i >= 0; --i)
    {
        auto & blade = mPhantomBlades[i];
        GameObject * pBlade = gameManager.GetGameObject(blade.phantomHandle);
        GameObject * pEnemy = gameManager.GetGameObject(blade.enemyHandle);

        if (!pEnemy)
        {
            pEnemy = pPlayerManager->FindClosestEnemy();
            if (pEnemy)
            {
                blade.enemyHandle = pEnemy->GetHandle();
            }
        }

        if (!pBlade || pBlade->IsDestroyed())
        {
            if (pBlade)
            {
                pBlade->Destroy();
            }
            mPhantomBlades.erase(mPhantomBlades.begin() + i);
            continue;
        }

        // Ghost trail logic
        mGhostTrails[blade.phantomHandle].push_back({ pBlade->GetPosition(), skGhostLifeTime });

        auto & ghosts = mGhostTrails[blade.phantomHandle];
        for (auto & ghost : ghosts)
        {
            ghost.timeLeft -= deltaTime;
        }
        ghosts.erase(std::remove_if(ghosts.begin(), ghosts.end(),
            [](const GhostSprite & g) { return g.timeLeft <= 0.f; }), ghosts.end());

        // Lifetime
        blade.timeLeft -= deltaTime;
        if (blade.timeLeft <= 0.f)
        {
            pBlade->Destroy();
            mPhantomBlades.erase(mPhantomBlades.begin() + i);
            continue;
        }

        // Movement toward enemy
        const sf::Vector2f & bladePos = pBlade->GetPosition();
        const sf::Vector2f & targetPos = pEnemy->GetPosition();
        sf::Vector2f toTarget = targetPos - bladePos;
        float dist = std::sqrt(toTarget.x * toTarget.x + toTarget.y * toTarget.y);

        if (dist > 1.0f)
        {
            sf::Vector2f dir = toTarget / dist;

            // Blade rotation: always face the player
            const sf::Vector2f & playerPos = GetGameObject().GetPosition();
            sf::Vector2f toPlayer = playerPos - bladePos;
            float angleRad = std::atan2(toPlayer.y, toPlayer.x);
            float angleDeg = angleRad * (180.f / BD::gsPi) + 90.f; // +90 to rotate handle forward
            if (b2Body * pBody = pBlade->GetPhysicsBody())
            {
                b2Vec2 position = pBody->GetPosition();
                float angleRadCorrected = angleRad - (BD::gsPi / 2.f); // Adjust to align handle visually
                pBody->SetTransform(position, angleRadCorrected);
            }

            // Physics movement
            if (b2Body * pBody = pBlade->GetPhysicsBody())
            {
                b2Vec2 velocity(
                    dir.x * mSpeed / pBlade->PIXELS_PER_METER,
                    dir.y * mSpeed / pBlade->PIXELS_PER_METER
                );
                pBody->SetLinearVelocity(velocity);
            }
            else
            {
                sf::Vector2f newPos = bladePos + dir * mSpeed * deltaTime;
                pBlade->SetPosition(newPos);
            }
        }
        else
        {
            GameObject * pNextEnemy = pPlayerManager->FindClosestEnemy();
            if (pNextEnemy)
            {
                blade.enemyHandle = pNextEnemy->GetHandle();
            }
        }
    }
}

//------------------------------------------------------------------------------------------------------------------------

void PhantomBladeComponent::DrawGhostTrails(sf::RenderWindow & window)
{
	auto & gameManager = GetGameManager();

	for (const auto & [handle, trail] : mGhostTrails)
	{
		GameObject * pBlade = gameManager.GetGameObject(handle);
		if (!pBlade) continue;

		auto pSpriteComp = pBlade->GetComponent<SpriteComponent>().lock();
		if (!pSpriteComp) continue;

		sf::Sprite baseSprite = pSpriteComp->GetSprite();
		for (const auto & ghost : trail)
		{
			float alpha = (ghost.timeLeft / skGhostLifeTime) * 150.f; // fade to transparent
			baseSprite.setColor(sf::Color(255, 255, 255, static_cast<sf::Uint8>(alpha)));
			baseSprite.setPosition(ghost.position);
			window.draw(baseSprite);
		}
	}
}

//------------------------------------------------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------------------------------------------------