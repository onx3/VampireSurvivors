#include "AstroidsPrivate.h"
#include "DamageZoneComponent.h"
#include "DamageComponent.h"
#include "PlayerManager.h"
#include "HealthComponent.h"

DamageZoneComponent::DamageZoneComponent(GameObject * pOwner, GameManager & gameManager, float damagePerSecond, float duration)
    : GameComponent(pOwner, gameManager)
    , mDamagePerSecond(damagePerSecond)
    , mLifetime(duration)
    , mDamageInterval(1.f)
    , mEnemyCooldowns()
    , mName("DamageZoneComponent")
{
}

//------------------------------------------------------------------------------------------------------------------------

void DamageZoneComponent::Update(float deltaTime)
{
    mLifetime -= deltaTime;
    if (mLifetime <= 0.f)
    {
        GetGameObject().Destroy();
        return;
    }

    for (auto & [handle, timer] : mEnemyCooldowns)
    {
        timer -= deltaTime;
    }

    auto * pManager = GetGameManager().GetManager<EnemyAIManager>();
    if (!pManager) 
    {
        return;
    }

    for (auto & enemyHandle : pManager->GetEnemies())
    {
        GameObject * pEnemy = GetGameManager().GetGameObject(enemyHandle);
        if (!pEnemy || pEnemy->IsDestroyed())
        {
            continue;
        }

        float distSqr = BD::GetMagnitudeSquared(GetGameObject().GetPosition(), pEnemy->GetPosition());
        float radius = 50.f;
        if (distSqr > radius * radius)
        {
            continue;
        }

        float & cooldown = mEnemyCooldowns[enemyHandle];
        if (cooldown <= 0.f)
        {
            auto dmg = pEnemy->GetComponent<HealthComponent>().lock();
            if (dmg)
            {
                dmg->LoseHealth(mDamagePerSecond);
                cooldown = mDamageInterval;
            }
        }
    }
}

//------------------------------------------------------------------------------------------------------------------------

void DamageZoneComponent::DebugImGuiComponentInfo()
{
}

//------------------------------------------------------------------------------------------------------------------------

std::string & DamageZoneComponent::GetClassName()
{
    return mName;
}

//------------------------------------------------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------------------------------------------------