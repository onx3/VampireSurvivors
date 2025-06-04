#include "AstroidsPrivate.h"
#include "PlayerManager.h"
#include "SpriteComponent.h"
#include "ControlledMovementComponent.h"
#include "ProjectileComponent.h"
#include "HealthComponent.h"
#include "CollisionComponent.h"
#include "ExplosionComponent.h"
#include <cassert>
#include "ResourceManager.h"
#include "CameraManager.h"
#include "FollowComponent.h"
#include "SwordSlashComponent.h"
#include "LevelManager.h"
#include "ThrowingKnife.h"
#include "PlayerStatsComponent.h"
#include "BoomerangComponent.h"
#include "PhantomBladeComponent.h"
#include "SpriteAnimationComponent.h"
#include "LightComponent.h"
#include "FirePotComponent.h"
#include "PlayerShootingComponent.h"
#include "WeaponComponent.h"

namespace
{
    static float sPlayerHealth = 100.f;
}

PlayerManager::PlayerManager(GameManager * pGameManager)
    : BaseManager(pGameManager)
    , mPlayerHandles()
    , mSpawnPos()
    , mpCurrentRoom(nullptr)
    , mLoseLifeSoundBuffer()
    , mDeathSoundBuffer()
    , mLoseLifeSound()
    , mDeathSound()
    , mSoundPlayed(false)
{
    auto * pLevelManager = GetGameManager().GetManager<LevelManager>();
    if (pLevelManager)
    {
        mSpawnPos = pLevelManager->GetLevelData().playerSpawnPosition;
        assert(!pLevelManager->GetLevelData().rooms.empty() && "No Rooms in current level");
    }
    InitPlayer();

    // Sound
    {
        mLoseLifeSoundBuffer.loadFromFile("../../VampireSurvivors/Audio/LoseLifeSound.wav");
        mLoseLifeSound.setBuffer(mLoseLifeSoundBuffer);
        mLoseLifeSound.setVolume(100.f);
        
        mDeathSoundBuffer.loadFromFile("../../VampireSurvivors/Audio/Death.flac");
        mDeathSound.setBuffer(mDeathSoundBuffer);
        mDeathSound.setVolume(50.f);
    }
}

//------------------------------------------------------------------------------------------------------------------------

PlayerManager::~PlayerManager()
{
}

//------------------------------------------------------------------------------------------------------------------------

void PlayerManager::InitPlayer()
{
    auto & gameManager = GetGameManager();
    BD::Handle playerHandle = gameManager.CreateNewGameObject(ETeam::Player, gameManager.GetRootGameObjectHandle());
    GameObject * pPlayer = gameManager.GetGameObject(playerHandle);

    mPlayerHandles.push_back(playerHandle);

    // Sprite Component
    {
        auto pSpriteComponent = pPlayer->GetComponent<SpriteComponent>().lock();
        if (pSpriteComponent)
        {
            std::string file = "../../VampireSurvivors/Art/Player/PlayerSpriteSheet.png";
            ResourceId resourceId(file);

            auto pTexture = gameManager.GetManager<ResourceManager>()->GetTexture(resourceId);
            if (pTexture)
            {
                pSpriteComponent->SetSprite(pTexture, sf::Vector2f(1.0f, 1.0f));
                pSpriteComponent->GetSprite().setTextureRect(sf::IntRect(0, 0, 6, 6));
                pSpriteComponent->GetSprite().setOrigin(8.f, 16.f);
                pSpriteComponent->SetPosition(mSpawnPos);
            }
        }
    }

    // Sprite Animation Component
    CreateAnimationComponent(*pPlayer);

    // Controlled Movement Component
    {
        auto pMovementComponent = pPlayer->GetComponent<ControlledMovementComponent>().lock();
        if (!pMovementComponent)
        {
            pMovementComponent = std::make_shared<ControlledMovementComponent>(pPlayer, gameManager);
            pPlayer->AddComponent(pMovementComponent);
        }
    }

    // Health Component
    {
        auto pHealthComponent = pPlayer->GetComponent<HealthComponent>().lock();
        if (!pHealthComponent)
        {
            pPlayer->AddComponent(std::make_shared<HealthComponent>(pPlayer, gameManager, sPlayerHealth, sPlayerHealth, 1, 1, .2f));
        }
    }

    // Collision Component
    {
        auto pCollisionComponent = pPlayer->GetComponent<CollisionComponent>().lock();
        if (!pCollisionComponent)
        {
            pPlayer->CreateBoxShapePhysicsBody(
                &gameManager.GetPhysicsWorld(),
                pPlayer->GetSize(),
                true,                  // isDynamic
                false                  // isSensor
            );

            auto * pBody = pPlayer->GetPhysicsBody();
            pBody->SetFixedRotation(true);

            pPlayer->AddComponent(std::make_shared<CollisionComponent>(
                pPlayer,
                gameManager,
                &gameManager.GetPhysicsWorld(),
                pBody,
                pPlayer->GetSize(),
                true
            ));
        }
    }

    // Player Shooting Component
    {
        auto pShootingComponent = pPlayer->GetComponent<PlayerShootingComponent>().lock();
        if (!pShootingComponent)
        {
            pShootingComponent = std::make_shared<PlayerShootingComponent>(pPlayer, gameManager);
            pPlayer->AddComponent(pShootingComponent);
        }
    }

    // Pistol Weapon
    {
        auto pWeaponComponent = pPlayer->GetComponent<WeaponComponent>().lock();
        if (!pWeaponComponent)
        {
            pWeaponComponent = std::make_shared<WeaponComponent>(pPlayer, gameManager, EWeaponType::Pistol);
            pPlayer->AddComponent(pWeaponComponent);
        }
    }

    // Player Stats Component
    {
        auto pPlayerStatsCompoennt = pPlayer->GetComponent<PlayerStatsComponent>().lock();
        if (!pPlayerStatsCompoennt)
        {
            pPlayerStatsCompoennt = std::make_shared<PlayerStatsComponent>(pPlayer, gameManager);
            pPlayer->AddComponent(pPlayerStatsCompoennt);
        }
    }

    // Light Component
    {
        auto pLightComponent = pPlayer->GetComponent<LightComponent>().lock();
        if (!pLightComponent)
        {
            pLightComponent = std::make_shared<LightComponent>(pPlayer, gameManager, 75.f, sf::Color(150, 200, 255, 180));
            pPlayer->AddComponent(pLightComponent);
        }
    }

    // Add default sword weapon GameObject to display
    AddWeaponGameObject(*pPlayer);

    // TESTING STUFF
    {
        /*{
            auto pThowingKnifeComponent = pPlayer->GetComponent<PhantomBladeComponent>().lock();
            if (!pThowingKnifeComponent)
            {
                pThowingKnifeComponent = std::make_shared<PhantomBladeComponent>(pPlayer, gameManager);
                pPlayer->AddComponent(pThowingKnifeComponent);
            }
        }*/
    }
}

//------------------------------------------------------------------------------------------------------------------------

void PlayerManager::Update(float deltaTime)
{
    auto & gameManager = GetGameManager();
    mPlayerHandles = gameManager.GetGameObjectsByTeam(ETeam::Player);

    if (mPlayerHandles.empty())
    {
        gameManager.EndGame();
    }
    else
    {
        for (auto playerHandle : mPlayerHandles)
        {
            GameObject * pPlayer = gameManager.GetGameObject(playerHandle);

            // Set correct room
            auto pLevelManager = gameManager.GetManager<LevelManager>();
            if (pLevelManager)
            {
                mpCurrentRoom = pLevelManager->GetRoomAtPosition(pPlayer->GetPosition());
            }

            // Destroy the player after the explosion animation finishes
            auto explosionComp = pPlayer->GetComponent<ExplosionComponent>().lock();
            if (explosionComp && explosionComp->IsAnimationFinished())
            {
                pPlayer->Destroy();
                return;
            }

            auto pHealthComp = pPlayer->GetComponent<HealthComponent>().lock();

            if (pHealthComp)
            {
                // Set the callbacks
                pHealthComp->SetLifeLostCallback([this, pPlayer]() {
                    OnPlayerLostLife(pPlayer);
                    });
                pHealthComp->SetDeathCallBack([this, pPlayer]() {
                    OnPlayerDeath(pPlayer);
                    });
            }
        }

        if (mLoseLifeSound.getStatus() == sf::Sound::Stopped)
        {
            mSoundPlayed = false;
        }
    }
}

//------------------------------------------------------------------------------------------------------------------------

void PlayerManager::OnGameEnd()
{
    mPlayerHandles.clear();
}

//------------------------------------------------------------------------------------------------------------------------

void PlayerManager::OnPlayerLostLife(GameObject * pPlayer)
{
    if (!mSoundPlayed)
    {
        mLoseLifeSound.play();
    }
}

//------------------------------------------------------------------------------------------------------------------------

void PlayerManager::OnPlayerDeath(GameObject * pPlayer)
{
    if (!mSoundPlayed)
    {
        mDeathSound.play();
        mSoundPlayed = true;
    }

    // Add the explosion animation here
    if (!pPlayer->GetComponent<ExplosionComponent>().lock())
    {
        auto explosionComp = std::make_shared<ExplosionComponent>(
            pPlayer, GetGameManager(), "../../VampireSurvivors/Art/explosion.png", 32, 32, 7, 0.1f, sf::Vector2f(2.f, 2.f), pPlayer->GetPosition());
        pPlayer->AddComponent(explosionComp);
    }
}

//------------------------------------------------------------------------------------------------------------------------

const std::vector<BD::Handle> & PlayerManager::GetPlayers() const
{
    return mPlayerHandles;
}

//------------------------------------------------------------------------------------------------------------------------

GameObject * PlayerManager::FindClosestEnemy()
{
    if (mPlayerHandles.empty())
    {
        return nullptr;
    }
    auto * pPlayer = GetGameManager().GetGameObject(mPlayerHandles[0]);

    const auto & myPos = pPlayer->GetPosition();
    float closestDistSq = std::numeric_limits<float>::max();
    GameObject * pEnemy = nullptr;

    auto & gameManager = GetGameManager();
    auto * pEnemyManager = gameManager.GetManager<EnemyAIManager>();

    if (pEnemyManager)
    {
        auto & enemies = pEnemyManager->GetEnemies();
        for (auto enemy : enemies)
        {
            auto * pObj = gameManager.GetGameObject(enemy);
            if (!pObj || pObj->IsDestroyed())
            {
                continue;
            }
            const sf::Vector2f & toEnemy = pObj->GetPosition() - myPos;
            float distSq = BD::Dot(toEnemy, toEnemy); // Gives length squared
            if (distSq < closestDistSq)
            {
                closestDistSq = distSq;
                pEnemy = pObj;
            }
        }
    }
    return pEnemy;
}

//------------------------------------------------------------------------------------------------------------------------

const RoomData * PlayerManager::GetCurrentRoom() const
{
    return mpCurrentRoom;
}

//------------------------------------------------------------------------------------------------------------------------

void PlayerManager::CreateAnimationComponent(GameObject & player)
{
    GameManager & gameManager = GetGameManager();
    auto pAnimComponent = player.GetComponent<SpriteAnimationComponent>().lock();
    if (!pAnimComponent)
    {
        pAnimComponent = std::make_shared<SpriteAnimationComponent>(&player, gameManager);
        player.AddComponent(pAnimComponent);

        // Create Idle animation (Top row: y = 0)
        Animation idleAnim;
        idleAnim.frames = {
            sf::IntRect(0,   0, 16, 28), // Frame 0
            sf::IntRect(16,  0, 16, 28), // Frame 1
            sf::IntRect(32,  0, 16, 28), // Frame 2
            sf::IntRect(48,  0, 16, 28)  // Frame 3
        };
        idleAnim.frameTime = 0.2f;

        // Create Move animation (Second row: y = 28)
        Animation moveAnim;
        moveAnim.frames = {
            sf::IntRect(0,   28, 16, 28), // Frame 0
            sf::IntRect(16,  28, 16, 28), // Frame 1
            sf::IntRect(32,  28, 16, 28), // Frame 2
            sf::IntRect(48,  28, 16, 28)  // Frame 3
        };
        moveAnim.frameTime = 0.15f;

        // Register the animations
        pAnimComponent->AddAnimation(EAnimationState::Idle, idleAnim);
        pAnimComponent->AddAnimation(EAnimationState::Move, moveAnim);

        // Start playing the Idle animation
        pAnimComponent->PlayAnimation(EAnimationState::Idle);
    }
}

//------------------------------------------------------------------------------------------------------------------------

void PlayerManager::AddWeaponGameObject(GameObject & player)
{
    GameManager & gameManager = GetGameManager();
    BD::Handle playerWeaponHandle = gameManager.CreateNewGameObject(ETeam::FriendlyPersistant, player.GetHandle());
    GameObject * pPlayerWeapon = gameManager.GetGameObject(playerWeaponHandle);

    // Weapon Sprite Component
    {
        auto pWeaponSpriteComponent = pPlayerWeapon->GetComponent<SpriteComponent>().lock();
        if (pWeaponSpriteComponent)
        {
            std::string file = "../../VampireSurvivors/Art/Weapons/Guns/Glock - P80 [64x48].png";
            ResourceId resourceId(file);

            auto pTexture = gameManager.GetManager<ResourceManager>()->GetTexture(resourceId);
            if (pTexture)
            {
                pWeaponSpriteComponent->SetSprite(pTexture, sf::Vector2f(.5f, .5f));
                pWeaponSpriteComponent->SetOriginToCenter();
                pWeaponSpriteComponent->SetPosition(player.GetPosition());
                pPlayerWeapon->SetRotation(pPlayerWeapon->GetRotationDegrees());
            }
        }
    }

    // Weapon Follow Component
    {
        auto pWeaponFollowComponent = pPlayerWeapon->GetComponent<FollowComponent>().lock();
        if (!pWeaponFollowComponent)
        {
            auto pWeaponFollowComponent = std::make_shared<FollowComponent>(pPlayerWeapon, gameManager, player.GetHandle(), sf::Vector2f(12, 10));
            pPlayerWeapon->AddComponent(pWeaponFollowComponent);
        }
    }
}

//------------------------------------------------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------------------------------------------------