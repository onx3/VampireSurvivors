#include "AstroidsPrivate.h"
#include "SpawnFromGroundEffectComponent.h"
#include "HealthComponent.h"
#include "EnemyMeleeAttackComponent.h"
#include "AISimplePathComponent.h"
#include "CollisionComponent.h"

SpawnFromGroundEffectComponent::SpawnFromGroundEffectComponent(GameObject * pOwner, GameManager & gameManager, EEnemy enemyType, BD::Handle targetHandle, float health, float duration)
    : GameComponent(pOwner, gameManager)
    , mDuration(duration)
    , mElapsed(0.f)
    , mHealth(health)
    , mSpawned(false)
    , mEnemyType(enemyType)
    , mTargetHandle(targetHandle)
    , mName("SpawnFromGroundEffectComponent")
{
    auto pSpriteComponent = GetGameObject().GetComponent<SpriteComponent>().lock();
    if (pSpriteComponent)
    {
        pSpriteComponent->SetScale({ 0.f, 0.f });
    }
}

//------------------------------------------------------------------------------------------------------------------------

void SpawnFromGroundEffectComponent::Update(float deltaTime)
{
    if (mSpawned)
    {
        return;
    }
    GameObject & gameObj = GetGameObject();
    mElapsed += deltaTime;
    float t = std::min(mElapsed / mDuration, 1.f);
    float scale = t * t * (3 - 2 * t);
    
    auto pSpriteComponent = gameObj.GetComponent<SpriteComponent>().lock();
    if (pSpriteComponent)
    {
        pSpriteComponent->SetScale(sf::Vector2f{ scale, scale });
    }

    if (t >= 1.f)
    {
        AddLogicComponents();
        mSpawned = true;
    }
}

//------------------------------------------------------------------------------------------------------------------------

void SpawnFromGroundEffectComponent::DebugImGuiComponentInfo()
{

}

//------------------------------------------------------------------------------------------------------------------------

std::string & SpawnFromGroundEffectComponent::GetClassName()
{
    return mName;
}

//------------------------------------------------------------------------------------------------------------------------

void SpawnFromGroundEffectComponent::AddLogicComponents()
{
    auto & gm = GetGameManager();
    auto * pEnemy = &GetGameObject();

    // AI
    auto pAI = std::make_shared<AISimplePathComponent>(pEnemy, gm, mTargetHandle);
    pEnemy->AddComponent(pAI);

    // Melee
    auto pMelee = std::make_shared<EnemyMeleeAttackComponent>(pEnemy, gm, mTargetHandle);
    pEnemy->AddComponent(pMelee);
}

//------------------------------------------------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------------------------------------------------