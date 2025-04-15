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


EnemyAIManager::EnemyAIManager(GameManager * pGameManager)
    : BaseManager(pGameManager)
    , mBaseEnemyCount(5)
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

            float healthMultiplier = .25f + runTime / 60.f;
            mCurrentHealth = mBaseHealth * std::min(healthMultiplier, 5.f); // Cap at 5x health

            int extraEnemies = int(runTime / 30.f); // +1 every 30s
            mCurrentMaxEnemies = mBaseEnemyCount + extraEnemies;
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

    auto pPlayerManager = GetGameManager().GetManager<PlayerManager>();
    if (!pPlayerManager)
    {
        return;
    }

    auto & players = pPlayerManager->GetPlayers();
    if (players.empty())
    {
        return;
    }

    BD::Handle playerHandle = players[0];
    GameObject * pPlayer = GetGameManager().GetGameObject(playerHandle);
    if (!pPlayer)
    {
        return;
    }

    auto playerPos = pPlayer->GetPosition();
    
    float minRadius = 600.f; // too close to player
    float maxRadius = 1000.f; // max spawn range

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> angleDist(0.f, 2.f * BD::gsPi);
    std::uniform_real_distribution<float> radiusDist(minRadius, maxRadius);

	while (mEnemyHandles.size() < mCurrentMaxEnemies)
	{
        float angle = angleDist(gen);
        float radius = radiusDist(gen);
        sf::Vector2f offset(std::cos(angle) * radius, std::sin(angle) * radius);
        sf::Vector2f spawnPosition = playerPos + offset;

		RespawnEnemy(EEnemy::Ogre, spawnPosition);
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
        {
            auto pPlayerManager = gameManager.GetManager<PlayerManager>();
            if (pPlayerManager)
            {
                auto & players = pPlayerManager->GetPlayers();
                if (!players.empty())
                {
                    BD::Handle playerHandle = players[0];
                    auto pAISimplePathComponentComp = std::make_shared<AISimplePathComponent>(pEnemy, gameManager, playerHandle);
                    pEnemy->AddComponent(pAISimplePathComponentComp);
                }
            }
        }

        // Health Component
        auto pHealthComponent = std::make_shared<HealthComponent>(pEnemy, gameManager, mCurrentHealth, mCurrentHealth, 1, 1);
        pEnemy->AddComponent(pHealthComponent);

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
        if (pEnemy && !pEnemy->IsDestroyed())
        {
            auto explosionComp = pEnemy->GetComponent<ExplosionComponent>().lock();
            if (explosionComp && explosionComp->IsAnimationFinished())
            {
                pEnemy->Destroy();
            }
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
		default:
		{
			return "Art/Enemies/Ogre/ogre_idle_anim_f0.png";
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
    // Explosion
    if (!pEnemy->GetComponent<ExplosionComponent>().lock())
    {
        auto explosionComp = std::make_shared<ExplosionComponent>(
            pEnemy, gameManager, "Art/explosion.png", 32, 32, 7, 0.1f, sf::Vector2f(2.f, 2.f), pEnemy->GetPosition());
        pEnemy->AddComponent(explosionComp);
    }
    // Add Score
    {
        auto pUIManager = gameManager.GetManager<UIManager>();
        pUIManager->AddScore(1000);
    }
    // Drops
    {
        EDropType dropType = DetermineDropType();
        auto pDropManager = gameManager.GetManager<DropManager>();
        if (pDropManager)
        {
            sf::Vector2f position = pEnemy->GetPosition();
            pDropManager->SpawnDrop(dropType, position);
        }
    }
}

//------------------------------------------------------------------------------------------------------------------------

EDropType EnemyAIManager::DetermineDropType() const
{
    int randomValue = rand() % 100;

    if (randomValue < 3)
    {
        return EDropType::NukePickup;
    }
    else if (randomValue < 7)
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
// EOF
//------------------------------------------------------------------------------------------------------------------------