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
    , mEnemiesInside()
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
        timer -= deltaTime;

    for (BD::Handle handle : mEnemiesInside)
    {
        GameObject * pEnemy = GetGameManager().GetGameObject(handle);
        if (!pEnemy || pEnemy->IsDestroyed()) continue;

        float & cooldown = mEnemyCooldowns[handle];
        if (cooldown <= 0.f)
        {
            auto health = pEnemy->GetComponent<HealthComponent>().lock();
            if (health)
            {
                health->LoseHealth(mDamagePerSecond);
                cooldown = mDamageInterval;
            }
        }
    }
}

//------------------------------------------------------------------------------------------------------------------------

void DamageZoneComponent::OnBeginOverlap(BD::Handle enemyHandle)
{
    mEnemiesInside.insert(enemyHandle);
    mEnemyCooldowns.try_emplace(enemyHandle, 0.f);
}

//------------------------------------------------------------------------------------------------------------------------

void DamageZoneComponent::OnEndOverlap(BD::Handle enemyHandle)
{
    mEnemiesInside.erase(enemyHandle);
    mEnemyCooldowns.erase(enemyHandle);
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