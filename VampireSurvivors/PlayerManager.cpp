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

static int sPlayerHealth = 100;

PlayerManager::PlayerManager(GameManager * pGameManager)
    : BaseManager(pGameManager)
    , mPlayerHandles()
    , mSoundPlayed(false)
{
    InitPlayer();

    // Sound
    {
        mLoseLifeSoundBuffer.loadFromFile("Audio/LoseLifeSound.wav");
        mLoseLifeSound.setBuffer(mLoseLifeSoundBuffer);
        mLoseLifeSound.setVolume(100.f);
        
        mDeathSoundBuffer.loadFromFile("Audio/Death.flac");
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
        sf::Vector2u windowSize = gameManager.GetWindow().getSize();
        sf::Vector2f centerPosition(float(windowSize.x) / 2.0f, float(windowSize.y) / 2.0f);

        auto pSpriteComponent = pPlayer->GetComponent<SpriteComponent>().lock();
        if (pSpriteComponent)
        {
            std::string file = "Art/Player.png";
            ResourceId resourceId(file);

            auto pTexture = gameManager.GetManager<ResourceManager>()->GetTexture(resourceId);
            if (pTexture)
            {
                pSpriteComponent->SetSprite(pTexture, sf::Vector2f(0.25f, 0.25f));
                pSpriteComponent->SetPosition(centerPosition);
            }
        }
    }

    // Controlled Movement Component
    {
        auto pMovementComponent = pPlayer->GetComponent<ControlledMovementComponent>().lock();
        if (!pMovementComponent)
        {
            auto pMovementComponent = std::make_shared<ControlledMovementComponent>(pPlayer, gameManager);
            pPlayer->AddComponent(pMovementComponent);
        }
    }

    // Projectile Component
    {
        auto pProjectileComponent = pPlayer->GetComponent<ProjectileComponent>().lock();
        if (!pProjectileComponent)
        {
            pPlayer->AddComponent(std::make_shared<ProjectileComponent>(pPlayer, gameManager));
        }
    }

    // Health Component
    {
        auto pHealthComponent = pPlayer->GetComponent<HealthComponent>().lock();
        if (!pHealthComponent)
        {
            pPlayer->AddComponent(std::make_shared<HealthComponent>(pPlayer, gameManager, sPlayerHealth, sPlayerHealth, 3, 3, 2.f));
        }
    }

    // Collision Component
    {
        auto pCollisionComponent = pPlayer->GetComponent<CollisionComponent>().lock();
        if (!pCollisionComponent)
        {
            pPlayer->CreatePhysicsBody(&gameManager.GetPhysicsWorld(), pPlayer->GetSize(), true);
            pPlayer->AddComponent(std::make_shared<CollisionComponent>(
                pPlayer,
                gameManager,
                &gameManager.GetPhysicsWorld(),
                pPlayer->GetPhysicsBody(),
                pPlayer->GetSize(),
                true
            ));
        }
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

            if (pHealthComp && pHealthComp->GetLives() == 1)
            {
                auto pSpriteComponent = pPlayer->GetComponent<SpriteComponent>().lock();
                if (pSpriteComponent)
                {
                    std::string file = "Art/playerDamaged.png";
                    ResourceId resourceId(file);

                    auto texture = gameManager.GetManager<ResourceManager>()->GetTexture(resourceId);
                    if (texture)
                    {
                        pSpriteComponent->SetSprite(texture, pSpriteComponent->GetSprite().getScale());
                    }
                }
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
            pPlayer, GetGameManager(), "Art/explosion.png", 32, 32, 7, 0.1f, sf::Vector2f(2.f, 2.f), pPlayer->GetPosition());
        pPlayer->AddComponent(explosionComp);
    }
}

//------------------------------------------------------------------------------------------------------------------------

const std::vector<BD::Handle> & PlayerManager::GetPlayers() const
{
    return mPlayerHandles;
}

//------------------------------------------------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------------------------------------------------