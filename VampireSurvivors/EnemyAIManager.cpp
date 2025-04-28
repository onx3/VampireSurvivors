#include "AstroidsPrivate.h"
#include "EnemyAIManager.h"
#include "GameObject.h"
#include <memory>
#include <cassert>
#include <iostream>
#include <random>
#include "SpriteComponent.h"
#include "CollisionComponent.h"
#include "HealthComponent.h"
#include "ExplosionComponent.h"
#include "DropManager.h"
#include "ResourceManager.h"
#include "AIPathComponent.h"
#include "FollowComponent.h"
#include "TrackingComponent.h"
#include "PlayerManager.h"
#include "EnemyBulletComponent.h"
#include "AISimplePathComponent.h"
#include "EnemyMeleeAttackComponent.h"
#include "LevelManager.h"


EnemyAIManager::EnemyAIManager(GameManager * pGameManager)
    : BaseManager(pGameManager)
    , mBaseEnemyCount(25)
    , mCurrentMaxEnemies(mBaseEnemyCount)
    , mBaseHealth(100.f)
    , mCurrentHealth(mBaseHealth)
    , mEnemyHandles()
{

}

//------------------------------------------------------------------------------------------------------------------------

EnemyAIManager::~EnemyAIManager()
{

}

//------------------------------------------------------------------------------------------------------------------------

void EnemyAIManager::Update(float deltaTime)
{
    GameManager & gameManager = GetGameManager();
    // Update mCurrentMaxEnemies and mCurrentHealth
    {
        auto pUIManager = gameManager.GetManager<UIManager>();
        if (pUIManager)
        {
            float runTime = pUIManager->GetRunTime();

            // Update health scaling
            float healthMultiplier = 0.25f + runTime / 60.f;
            mCurrentHealth = mBaseHealth * std::min(healthMultiplier, 15.f); // Cap at 15x health

            float growthRate = 1.0215f;
            mCurrentMaxEnemies = int(mBaseEnemyCount * std::pow(growthRate, runTime));

            mCurrentMaxEnemies = std::min(mCurrentMaxEnemies, 120);
            if (mCurrentMaxEnemies == 120)
            {
                int ii = 0;
            }
        }
    }

    for (auto enemyHandle : mEnemyHandles)
    {
        auto * pEnemy = GetGameManager().GetGameObject(enemyHandle);
        if (pEnemy && !pEnemy->IsDestroyed())
        {
            auto pHealthComp = pEnemy->GetComponent<HealthComponent>().lock();
            if (pHealthComp)
            {
                pHealthComp->SetDeathCallBack([this, enemyHandle]() {
                    GameObject * pEnemySafe = GetGameManager().GetGameObject(enemyHandle);
                    if (pEnemySafe)
                    {
                        OnDeath(pEnemySafe);
                    }
                    });
            }
        }
    }
	CleanUpDeadEnemies();

    auto pPlayerManager = gameManager.GetManager<PlayerManager>();
    if (!pPlayerManager || pPlayerManager->GetPlayers().empty())
        return;

    BD::Handle playerHandle = pPlayerManager->GetPlayers()[0];
    GameObject * pPlayer = gameManager.GetGameObject(playerHandle);
    if (!pPlayer)
        return;

    sf::Vector2f playerPos = pPlayer->GetPosition();

    float minRadius = 600.f;
    float maxRadius = 1000.f;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> angleDist(0.f, 2.f * BD::gsPi);
    std::uniform_real_distribution<float> radiusDist(minRadius, maxRadius);

    auto * pLevelManager = gameManager.GetManager<LevelManager>();
    if (!pLevelManager)
        return;

    int attempts = 0;
    const int maxAttempts = 100;

    while (mEnemyHandles.size() < mCurrentMaxEnemies && attempts < maxAttempts)
    {
        ++attempts;

        float angle = angleDist(gen);
        float radius = radiusDist(gen);
        sf::Vector2f offset(std::cos(angle) * radius, std::sin(angle) * radius);
        sf::Vector2f spawnPosition = playerPos + offset;

        if (pLevelManager->IsTileWalkableAI(int(spawnPosition.x / BD::gsPixelCountCellSize) , int(spawnPosition.y / BD::gsPixelCountCellSize)))
        {
            EEnemy EnemyType = GetEnemyType();
            RespawnEnemy(EnemyType, spawnPosition);
        }
    }
}

//------------------------------------------------------------------------------------------------------------------------

void EnemyAIManager::OnGameEnd()
{
    mEnemyHandles.clear();
}

//------------------------------------------------------------------------------------------------------------------------

void EnemyAIManager::RemoveEnemy(GameObject * pEnemy)
{
    pEnemy->Destroy();
}

//------------------------------------------------------------------------------------------------------------------------

void EnemyAIManager::RespawnEnemy(EEnemy type, sf::Vector2f pos)
{
	AddEnemies(1, type, pos);
}

//------------------------------------------------------------------------------------------------------------------------

void EnemyAIManager::AddEnemies(int count, EEnemy type, sf::Vector2f pos)
{
    auto & gameManager = GetGameManager();
    for (int i = 0; i < count; ++i)
    {
        BD::Handle enemyHandle = gameManager.CreateNewGameObject(ETeam::Enemy, gameManager.GetRootGameObjectHandle());
        auto * pEnemy = gameManager.GetGameObject(enemyHandle);
        mEnemyHandles.push_back(enemyHandle);

        auto pSpriteComp = pEnemy->GetComponent<SpriteComponent>().lock();

        if (!pSpriteComp)
        {
            return;
        }
        // Sprite Comp
        SetUpSprite(*pSpriteComp, type);
        pSpriteComp->SetPosition(pos);

        // AI Simple Path Movement
        BD::Handle playerHandle;
        {
            auto pPlayerManager = gameManager.GetManager<PlayerManager>();
            if (pPlayerManager)
            {
                auto & players = pPlayerManager->GetPlayers();
                if (!players.empty())
                {
                    playerHandle = players[0];
                    auto pAISimplePathComponentComp = std::make_shared<AISimplePathComponent>(pEnemy, gameManager, playerHandle);
                    pEnemy->AddComponent(pAISimplePathComponentComp);
                }
            }
        }

        // Health Component
        auto pHealthComponent = std::make_shared<HealthComponent>(pEnemy, gameManager, mCurrentHealth, mCurrentHealth, 1, 1);
        pEnemy->AddComponent(pHealthComponent);

        // MeleeAttackComponent
        auto pMeleeAttackComponent = std::make_shared<EnemyMeleeAttackComponent>(pEnemy, gameManager, playerHandle);
        pEnemy->AddComponent(pMeleeAttackComponent);

        // Physics and Collision
        {
            pEnemy->CreateBoxShapePhysicsBody(&gameManager.GetPhysicsWorld(), pEnemy->GetSize(), true);
            auto pCollisionComp = std::make_shared<CollisionComponent>(
                pEnemy,
                gameManager,
                &gameManager.GetPhysicsWorld(),
                pEnemy->GetPhysicsBody(),
                pEnemy->GetSize(),
                true
            );
            pEnemy->AddComponent(pCollisionComp);
        }
    }
}

//------------------------------------------------------------------------------------------------------------------------

void EnemyAIManager::DestroyAllEnemies()
{
    for (auto enemyHandle : mEnemyHandles)
    {
        auto * pEnemy = GetGameManager().GetGameObject(enemyHandle);
        pEnemy->Destroy();
    }
}

//------------------------------------------------------------------------------------------------------------------------

void EnemyAIManager::CleanUpDeadEnemies()
{
    auto & gameManager = GetGameManager();

    for (BD::Handle enemyHandle : mEnemyHandles)
    {
        GameObject * pEnemy = gameManager.GetGameObject(enemyHandle);
        if (pEnemy && !pEnemy->IsDestroyed() && !pEnemy->IsActive())
        {
            pEnemy->Destroy();
        }
    }

    auto removeStart = std::remove_if(mEnemyHandles.begin(), mEnemyHandles.end(),
        [&gameManager](BD::Handle handle)
        {
            GameObject * pObj = gameManager.GetGameObject(handle);
            return pObj == nullptr || pObj->IsDestroyed();
        });

    mEnemyHandles.erase(removeStart, mEnemyHandles.end());
}

//------------------------------------------------------------------------------------------------------------------------

std::string EnemyAIManager::GetEnemyFile(EEnemy type)
{
	switch (type)
	{
        case (EEnemy::LizardF):
        {
            return "Art/Enemies/LizardF/lizard_f_idle_anim_f0.png";
        }
        case (EEnemy::Ogre):
        {
            return "Art/Enemies/Ogre/ogre_idle_anim_f0.png";
        }
        case (EEnemy::Chort):
        {
            return "Art/Enemies/Chort/chort_run_anim_f0.png";
        }
		default:
		{
			return "Art/Enemies/Chort/chort_run_anim_f0.png";
		}
	}
}

//------------------------------------------------------------------------------------------------------------------------

const std::vector<BD::Handle> & EnemyAIManager::GetEnemies() const
{
	return mEnemyHandles;
}

//------------------------------------------------------------------------------------------------------------------------

void EnemyAIManager::OnDeath(GameObject * pEnemy)
{
    auto & gameManager = GetGameManager();
    auto pDropManager = gameManager.GetManager<DropManager>();
    sf::Vector2f position = pEnemy->GetPosition();

    // Drop Coins
    {
        pDropManager->DropCoins(position);
    }
    // Drops
    {
        EDropType dropType = DetermineDropType();
        if (pDropManager)
        {
            pDropManager->SpawnDrop(dropType, position);
        }
    }
}

//------------------------------------------------------------------------------------------------------------------------

EDropType EnemyAIManager::DetermineDropType() const
{
    int randomValue = rand() % 100;

    if (randomValue < 7)
    {
        return EDropType::LifePickup;
    }

    return EDropType::None;
}

//------------------------------------------------------------------------------------------------------------------------

void EnemyAIManager::SetUpSprite(SpriteComponent & spriteComp, EEnemy type)
{
    std::string file = GetEnemyFile(type);
    ResourceId resourceId(file);
    auto pTexture = GetGameManager().GetManager<ResourceManager>()->GetTexture(resourceId);

    auto scale = sf::Vector2f();
    switch (type)
    {
        case (EEnemy::LizardF):
        {
            scale = sf::Vector2f(1.2f, 1.2f);
            break;
        }
        case (EEnemy::Ogre):
        {
            scale = sf::Vector2f(1.2f, 1.2f);
            break;
        }
        default :
        {
            scale = sf::Vector2f(1.2f, 1.2f);
            break;
        }
    }
    spriteComp.SetSprite(pTexture, scale);
}

//------------------------------------------------------------------------------------------------------------------------

EEnemy EnemyAIManager::GetEnemyType()
{
    int enemyCount = int(EEnemy::Total);

    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, enemyCount - 1);
    int randomIndex = dist(gen);
    return EEnemy(randomIndex);
}

//------------------------------------------------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------------------------------------------------