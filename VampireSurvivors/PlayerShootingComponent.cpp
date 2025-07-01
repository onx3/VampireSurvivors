#include "AstroidsPrivate.h"
#include "PlayerShootingComponent.h"
#include "CameraManager.h"
#include "CollisionComponent.h"
#include "HealthComponent.h"
#include "AudioManager.h"
#include "WeaponComponent.h"
#include "WeaponInventoryComponent.h"

PlayerShootingComponent::PlayerShootingComponent(GameObject * pOwner, GameManager & gameManager)
    : GameComponent(pOwner, gameManager)
    , mEmptyGunSoundCoolDown(.5f)
    , mEmptyGunSoundTimer(0.f)
    , mLastTracerStart()
    , mLastTracerEnd()
    , mTracerTimer()
    , mTracerLifespan(.0f)
    , mName("PlayerShootingComponent")
{

}

//------------------------------------------------------------------------------------------------------------------------

void PlayerShootingComponent::Update(float deltaTime)
{
    GameManager & gameManager = GetGameManager();
    mEmptyGunSoundTimer -= deltaTime;

    auto pInventory = GetGameObject().GetComponent<WeaponInventoryComponent>().lock();

    if (pInventory)
    {
        if (gameManager.mInputHandler.IsKeyJustPressed(sf::Keyboard::R))
        {
            pInventory->Reload();
        }
        if (gameManager.mInputHandler.IsKeyJustPressed(sf::Keyboard::E))
        {
            pInventory->SwitchToNextSlot();
        }

        if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
        {
            pInventory->Shoot();

            if (!pInventory->IsReloading() && mEmptyGunSoundTimer <= 0.f && pInventory->GetAmmoInCurrentClip() <= 0)
            {
                // Empty Gun sound
                mEmptyGunSoundTimer = mEmptyGunSoundCoolDown;
                ResourceId resId = ResourceId("../../VampireSurvivors/Audio/EmptyGun.mp3");
                auto pBuffer = gameManager.GetManager<ResourceManager>()->GetSoundBuffer(resId);
                if (pBuffer)
                {
                    gameManager.GetManager<AudioManager>()->PlayPooledSound(pBuffer, 10.f, 1.f);
                    pInventory->Reload();
                }
            }
        }
    }
}

//------------------------------------------------------------------------------------------------------------------------

void PlayerShootingComponent::draw(sf::RenderTarget & target, sf::RenderStates states)
{
    if (mTracerTimer <= 0.f)
    {
        return;
    }

    float alpha = (mTracerTimer / mTracerLifespan) * 255.f;

    sf::VertexArray line(sf::Lines, 2);
    line[0].position = mLastTracerStart;
    line[1].position = mLastTracerEnd;

    sf::Color tracerColor = sf::Color::White;
    tracerColor.a = static_cast<sf::Uint8>(alpha);
    line[0].color = line[1].color = tracerColor;

    target.draw(line);
}

//------------------------------------------------------------------------------------------------------------------------

void PlayerShootingComponent::DebugImGuiComponentInfo()
{

}

//------------------------------------------------------------------------------------------------------------------------

const std::string & PlayerShootingComponent::GetClassName()
{
    return mName;
}

//------------------------------------------------------------------------------------------------------------------------

void PlayerShootingComponent::Shoot()
{
    GameManager & gameManager = GetGameManager();
    const sf::Vector2f & startPos = GetGameObject().GetPosition();
    const sf::Vector2f & endPos = gameManager.GetManager<CameraManager>()->GetCrosshairPosition();

    GameObject * hitEnemy = nullptr;
    std::vector<GameObject *> objects;
    gameManager.GetAllGameObjects(objects);
    for (GameObject * pObj : objects)
    {
        if (pObj->GetTeam() == ETeam::Enemy && !pObj->IsDestroyed())
        {
            auto pCollision = pObj->GetComponent<CollisionComponent>().lock();
            if (pCollision && pCollision->IntersectsLine(startPos, endPos))
            {
                hitEnemy = pObj;
                break;
            }
        }
    }

    if (hitEnemy)
    {
        auto pHealth = hitEnemy->GetComponent<HealthComponent>().lock();
        if (pHealth)
        {
            pHealth->LoseHealth(25.f);
        }
    }

    // Fire sound
    ResourceId resId = ResourceId("../../VampireSurvivors/Audio/9mm.mp3");
    auto pBuffer = gameManager.GetManager<ResourceManager>()->GetSoundBuffer(resId);
    if (pBuffer)
    {
        gameManager.GetManager<AudioManager>()->PlayPooledSound(pBuffer, 20.f, 1.f);
    }

    mLastTracerStart = startPos;
    mLastTracerEnd = endPos;
    mTracerTimer = mTracerLifespan;
}

//------------------------------------------------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------------------------------------------------\