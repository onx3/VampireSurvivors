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
#include "TrackingComponent.h"
#include "PlayerManager.h"
#include "EnemyBulletComponent.h"
#include "AISimplePathComponent.h"
#include "EnemyMeleeAttackComponent.h"
#include "LevelManager.h"
#include "SpriteAnimationComponent.h"
#include "SpawnFromGroundEffectComponent.h"


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

            //float growthRate = 1.0215f;
            float growthRate = 1.015f;
            mCurrentMaxEnemies = int(mBaseEnemyCount * std::pow(growthRate, runTime));

            mCurrentMaxEnemies = std::min(mCurrentMaxEnemies, 30);
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

    auto * pLevelManager = gameManager.GetManager<LevelManager>();
    if (!pLevelManager)
    {
        return;
    }
    auto * pPlayerManager = gameManager.GetManager<PlayerManager>();
    if (!pPlayerManager)
    {
        return;
    }

    const LevelData & levelData = pLevelManager->GetLevelData();
    const RoomData * pRoomData = pPlayerManager->GetCurrentRoom();

    if (!pRoomData)
    {
        return;
    }

    while (mEnemyHandles.size() < mCurrentMaxEnemies)
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dist(0, int(pRoomData->enemySpawnPositions.size()) - 1);
        int randomIndex = dist(gen);

        sf::Vector2f spawnPosition = pRoomData->enemySpawnPositions[randomIndex];

        if (pLevelManager->IsTileWalkableAI(int(spawnPosition.x / BD::gsPixelCountCellSize), int(spawnPosition.y / BD::gsPixelCountCellSize)))
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

        BD::Handle playerHandle;
        {
            auto pPlayerManager = gameManager.GetManager<PlayerManager>();
            if (pPlayerManager)
            {
                auto & players = pPlayerManager->GetPlayers();
                if (!players.empty())
                {
                    playerHandle = players[0];
                }
            }
        }

        auto pSpawnFromGroundEffectComponent = std::make_shared<SpawnFromGroundEffectComponent>(pEnemy, gameManager, type, playerHandle, mCurrentHealth);
        pEnemy->AddComponent(pSpawnFromGroundEffectComponent);

        auto pSpriteComp = pEnemy->GetComponent<SpriteComponent>().lock();

        if (!pSpriteComp)
        {
            return;
        }
        // Sprite Comp
        SetUpSprite(*pEnemy, *pSpriteComp, type);
        pSpriteComp->SetPosition(pos);
        sf::Vector2f fullSize = sf::Vector2f(
            float(pSpriteComp->GetSprite().getTextureRect().width),
            float(pSpriteComp->GetSprite().getTextureRect().height)
        );

        // Health
        auto pHealth = std::make_shared<HealthComponent>(pEnemy, gameManager, mCurrentHealth, mCurrentHealth, 1, 1);
        pEnemy->AddComponent(pHealth);

        // Physics and Collision
        {
            pEnemy->CreateBoxShapePhysicsBody(
                &gameManager.GetPhysicsWorld(),
                fullSize,
                true,                // isDynamic
                false                // isSensor
            );

            auto * pBody = pEnemy->GetPhysicsBody();
            pBody->SetFixedRotation(true);

            auto pCollisionComp = std::make_shared<CollisionComponent>(
                pEnemy,
                gameManager,
                &gameManager.GetPhysicsWorld(),
                pEnemy->GetPhysicsBody(),
                fullSize,
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
            return "../../VampireSurvivors/Art/Enemies/LizardF/LizardFSpriteSheet.png";
        }
        case (EEnemy::Ogre):
        {
            return "../../VampireSurvivors/Art/Enemies/Ogre/OgreSpriteSheet.png";
        }
        case (EEnemy::Chort):
        {
            return "../../VampireSurvivors/Art/Enemies/Chort/ChortSpriteSheet.png";
        }
        default:
        {
            return "../../VampireSurvivors/Art/Enemies/Chort/ChortSpriteSheet.png";
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

void EnemyAIManager::SetUpSprite(GameObject & gameObj, SpriteComponent & spriteComp, EEnemy type)
{
    std::string file = GetEnemyFile(type);
    ResourceId resourceId(file);
    auto pTexture = GetGameManager().GetManager<ResourceManager>()->GetTexture(resourceId);

    auto scale = sf::Vector2f(0.f, 0.f); // Default scale

    int frameWidth = 0;
    int frameHeight = 0;

    switch (type)
    {
        case (EEnemy::LizardF):
        {
            scale = sf::Vector2f(1.2f, 1.2f);
            frameWidth = 16;
            frameHeight = 28;
            break;
        }
        case (EEnemy::Ogre):
        {
            scale = sf::Vector2f(1.2f, 1.2f);
            frameWidth = 32;
            frameHeight = 35;
            break;
        }
        case (EEnemy::Chort):
        default:
        {
            scale = sf::Vector2f(1.2f, 1.2f);
            frameWidth = 16;
            frameHeight = 22;
            break;
        }
    }

    spriteComp.SetSprite(pTexture, scale);

    spriteComp.GetSprite().setOrigin(frameWidth * 0.5f, frameHeight - 12.f); // 12 just seems right

    auto pAnimComponent = gameObj.GetComponent<SpriteAnimationComponent>().lock();
    if (!pAnimComponent)
    {
        pAnimComponent = std::make_shared<SpriteAnimationComponent>(&gameObj, GetGameManager());
        gameObj.AddComponent(pAnimComponent);
    }

    // Setup Move animation only
    Animation moveAnim;
    moveAnim.frameTime = 0.15f;

    moveAnim.frames = {
        sf::IntRect(0, 0, frameWidth, frameHeight),
        sf::IntRect(frameWidth, 0, frameWidth, frameHeight)
    };

    pAnimComponent->AddAnimation(EAnimationState::Move, moveAnim);
    pAnimComponent->PlayAnimation(EAnimationState::Move);
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