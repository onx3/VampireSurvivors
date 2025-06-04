#include "AstroidsPrivate.h"
#include "WeaponComponent.h"
#include "AudioManager.h"
#include "CameraManager.h"
#include "CollisionComponent.h"
#include "HealthComponent.h"

WeaponComponent::WeaponComponent(GameObject * pOwner, GameManager & gameManager, EWeaponType weaponType)
    : GameComponent(pOwner, gameManager)
    , mWeaponType(weaponType)
    , mReloadTimer(0.f)
    , mIsReloading(false)
    , mFireCooldown(0.f)
    , mName("WeaponComponent")
{
    switch (mWeaponType)
    {
        case (EWeaponType::Pistol):
        {
            mClipSize = 10;
            mReserveAmmo = -1; // infinite
            mReloadTime = 1.5f;
            mTimeBetweenShots = 0.3f;
            break;
        }

        case (EWeaponType::SMG):
        {
            mClipSize = 25;
            mReserveAmmo = 150;
            mReloadTime = 1.7f;
            mTimeBetweenShots = .1f;
            break;
        }

        case (EWeaponType::Shotgun):
        {
            mClipSize = 5;
            mReserveAmmo = 25;
            mReloadTime = 2.5f;
            mTimeBetweenShots = 1.0f;
            break;
        }

        case (EWeaponType::Rifle):
        {
            mClipSize = 30;
            mReserveAmmo = 90;
            mReloadTime = 2.0f;
            mTimeBetweenShots = 0.2f;
            break;
        }
    }

    mClipAmmo = mClipSize;
}

//------------------------------------------------------------------------------------------------------------------------

WeaponComponent::~WeaponComponent()
{
}

//------------------------------------------------------------------------------------------------------------------------

void WeaponComponent::Update(float deltaTime)
{
    if (mIsReloading)
    {
        mReloadTimer -= deltaTime;
        if (mReloadTimer <= 0.f)
        {
            int ammoNeeded = mClipSize - mClipAmmo;
            int toReload = (mReserveAmmo == -1) ? ammoNeeded : std::min(ammoNeeded, mReserveAmmo);
            mClipAmmo += toReload;
            if (mReserveAmmo != -1)
                mReserveAmmo -= toReload;

            mIsReloading = false;
        }
    }

    if (mFireCooldown > 0.f)
    {
        mFireCooldown -= deltaTime;
    }
}

//------------------------------------------------------------------------------------------------------------------------

void WeaponComponent::draw(sf::RenderTarget & target, sf::RenderStates states)
{
}

//------------------------------------------------------------------------------------------------------------------------

void WeaponComponent::DebugImGuiComponentInfo()
{
}

//------------------------------------------------------------------------------------------------------------------------

std::string & WeaponComponent::GetClassName()
{
    return mName;
}

//------------------------------------------------------------------------------------------------------------------------

void WeaponComponent::Shoot()
{
    if (!CanShoot())
    {
        return;
    }

    // Fire projectile or trigger effect here
    ConsumeAmmo();

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

    // Add shoot sound here
    auto pAudioManager = GetGameManager().GetManager<AudioManager>();
    if (pAudioManager)
    {
        ResourceId soundId("../../VampireSurvivors/Audio/9mm.mp3");
        auto pBuffer = GetGameManager().GetManager<ResourceManager>()->GetSoundBuffer(soundId);
        if (pBuffer)
            pAudioManager->PlayPooledSound(pBuffer, 20.f, 1.f);
    }

    mFireCooldown = mTimeBetweenShots;
}

//------------------------------------------------------------------------------------------------------------------------

void WeaponComponent::StartReload()
{
    if (mIsReloading || mClipAmmo == mClipSize || (mReserveAmmo == 0))
    {
        return;
    }

    mIsReloading = true;
    mReloadTimer = mReloadTime;

    // Play reload sound
    auto pAudioManager = GetGameManager().GetManager<AudioManager>();
    if (pAudioManager)
    {
        ResourceId reloadId("../../VampireSurvivors/Audio/Reload.mp3");
        auto pBuffer = GetGameManager().GetManager<ResourceManager>()->GetSoundBuffer(reloadId);
        if (pBuffer)
            pAudioManager->PlayPooledSound(pBuffer, 20.f, 1.f);
    }
}

//------------------------------------------------------------------------------------------------------------------------

int WeaponComponent::GetClipSize() const
{
    return mClipSize;
}

//------------------------------------------------------------------------------------------------------------------------

int WeaponComponent::GetAmmoInClip() const
{
    return mClipAmmo;
}

//------------------------------------------------------------------------------------------------------------------------

int WeaponComponent::GetReserveAmmo() const
{
    return mReserveAmmo;
}

//------------------------------------------------------------------------------------------------------------------------

EWeaponType WeaponComponent::GetWeaponType() const
{
    return mWeaponType;
}

//------------------------------------------------------------------------------------------------------------------------

bool WeaponComponent::IsReloading() const
{
    return mIsReloading;
}

//------------------------------------------------------------------------------------------------------------------------

void WeaponComponent::ConsumeAmmo()
{
    if (mClipAmmo > 0)
    {
        --mClipAmmo;
    }
}

//------------------------------------------------------------------------------------------------------------------------

bool WeaponComponent::CanShoot() const
{
    return !mIsReloading && mClipAmmo > 0 && mFireCooldown <= 0.f;
}

//------------------------------------------------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------------------------------------------------