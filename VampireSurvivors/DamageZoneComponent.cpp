#include "AstroidsPrivate.h"
#include "DamageZoneComponent.h"
#include "DamageComponent.h"
#include "PlayerManager.h"
#include "HealthComponent.h"
#include "PlayerStatsComponent.h"

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
    GameManager & gameManager = GetGameManager();
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
        GameObject * pEnemy = gameManager.GetGameObject(handle);
        if (!pEnemy || pEnemy->IsDestroyed())
        {
            continue;
        }

        float & cooldown = mEnemyCooldowns[handle];
        if (cooldown <= 0.f)
        {
            auto pHealthComponent = pEnemy->GetComponent<HealthComponent>().lock();
            GameObject * pPlayerObj = nullptr;
            if (pHealthComponent)
            {
                auto * pPlayerManager = gameManager.GetManager<PlayerManager>();
                if (pPlayerManager)
                {
                    auto & players = pPlayerManager->GetPlayers();
                    if (!players.empty())
                    {
                        pPlayerObj = gameManager.GetGameObject(players[0]);
                    }
                }

                float overalDamageMult = 1.0f;
                if (pPlayerObj)
                {
                    auto pPlayerStatsComp = pPlayerObj->GetComponent<PlayerStatsComponent>().lock();
                    if (pPlayerStatsComp)
                    {
                        overalDamageMult = pPlayerStatsComp->GetDamageMult();
                    }
                }

                pHealthComponent->LoseHealth(mDamagePerSecond * overalDamageMult);
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