#include "AstroidsPrivate.h"
#include "EnemyMeleeAttackComponent.h"
#include "HealthComponent.h"

EnemyMeleeAttackComponent::EnemyMeleeAttackComponent(GameObject * pGameObject, GameManager & gameManager, BD::Handle & playerHandle)
	: GameComponent(pGameObject, gameManager)
	, mAttackState(EAttackState::Idle)
	, mAttackRange(60.f)
	, mWindUpTime(.5f)
	, mTimer(0.f)
	, mDamage(10.f)
	, mPlayerHandle(playerHandle)
	, mName("EnemyMeleeAttackComponent")
{
}

//------------------------------------------------------------------------------------------------------------------------

EnemyMeleeAttackComponent::~EnemyMeleeAttackComponent()
{
}

//------------------------------------------------------------------------------------------------------------------------

void EnemyMeleeAttackComponent::Update(float deltaTime)
{
	auto & gameManager = GetGameManager();
	GameObject * pPlayerObj = gameManager.GetGameObject(mPlayerHandle);
	
	if (!pPlayerObj)
	{
		return;
	}

	sf::Vector2f enemyPos = GetGameObject().GetPosition();
	sf::Vector2f playerPos = pPlayerObj->GetPosition();
	sf::Vector2f direction = enemyPos - playerPos;
	float distanceSquared = BD::GetMagnitudeSquared(direction);

	switch (mAttackState)
	{
		case (EAttackState::Idle):
		{
			if (distanceSquared < mAttackRange * mAttackRange)
			{
				mTimer = mWindUpTime;
				mAttackState = EAttackState::WindUp;
			}
			break;
		}

		case (EAttackState::WindUp):
		{
			mTimer -= deltaTime;
			if (mTimer <= 0.f)
			{
				if (distanceSquared < mAttackRange * mAttackRange)
				{
					auto pHealthComp = pPlayerObj->GetComponent<HealthComponent>().lock();
					if (pHealthComp)
					{
						pHealthComp->LoseHealth(mDamage);
					}
					mTimer = mAttackCooldown;
					mAttackState = EAttackState::Cooldown;
				}
			}
			break;
		}

		case (EAttackState::Cooldown):
		{
			mTimer -= deltaTime;
			if (mTimer <= 0.f)
			{
				mAttackState = EAttackState::Idle;
			}
			break;
		}

		default:
			break;
	}
}

//------------------------------------------------------------------------------------------------------------------------

void EnemyMeleeAttackComponent::draw(sf::RenderTarget & target, sf::RenderStates states)
{
}

//------------------------------------------------------------------------------------------------------------------------

void EnemyMeleeAttackComponent::DebugImGuiComponentInfo()
{
}

//------------------------------------------------------------------------------------------------------------------------

std::string & EnemyMeleeAttackComponent::GetClassName()
{
	return mName;
}

//------------------------------------------------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------------------------------------------------