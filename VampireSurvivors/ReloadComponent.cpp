#include "AstroidsPrivate.h"
#include "ReloadComponent.h"
#include "AudioManager.h"

ReloadComponent::ReloadComponent(GameObject * pOwner, GameManager & gameManger, int clipSize, int reserveAmmo, float reloadTime)
    : GameComponent(pOwner, gameManger)
    , mClipAmmo(clipSize)
    , mClipSize(clipSize)
    , mReserveAmmo(reserveAmmo)
    , mReloadTime(reloadTime)
    , mReloadTimer(0.f)
    , mIsReloading(false)
    , mName("ReloadComponent")
{

}

//------------------------------------------------------------------------------------------------------------------------

ReloadComponent::~ReloadComponent()
{

}

//------------------------------------------------------------------------------------------------------------------------

void ReloadComponent::Update(float deltaTime)
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
            {
                mReserveAmmo -= toReload;
            }
            mIsReloading = false;
        }
    }
}

//------------------------------------------------------------------------------------------------------------------------

void ReloadComponent::draw(sf::RenderTarget & target, sf::RenderStates states)
{

}

//------------------------------------------------------------------------------------------------------------------------

void ReloadComponent::DebugImGuiComponentInfo()
{

}

//------------------------------------------------------------------------------------------------------------------------

std::string & ReloadComponent::GetClassName()
{
    return mName;
}

//------------------------------------------------------------------------------------------------------------------------

bool ReloadComponent::CanShoot() const
{
    return !mIsReloading && mClipAmmo > 0;
}

//------------------------------------------------------------------------------------------------------------------------

void ReloadComponent::ConsumeAmmo()
{
    if (mClipAmmo > 0)
    {
        --mClipAmmo;
    }
}

//------------------------------------------------------------------------------------------------------------------------

void ReloadComponent::StartReload()
{
    GameManager & gameManager = GetGameManager();
    if (!mIsReloading && mClipAmmo < mClipSize && (mReserveAmmo == -1 || mReserveAmmo > 0))
    {
        mIsReloading = true;
        mReloadTimer = mReloadTime;

        // Fire sound
        ResourceId resId = ResourceId("../../VampireSurvivors/Audio/Reload.mp3");
        auto pBuffer = gameManager.GetManager<ResourceManager>()->GetSoundBuffer(resId);
        if (pBuffer)
        {
            gameManager.GetManager<AudioManager>()->PlayPooledSound(pBuffer, 20.f, 1.f);
        }
    }
}

//------------------------------------------------------------------------------------------------------------------------

bool ReloadComponent::IsReloading() const
{
    return mIsReloading;
}

//------------------------------------------------------------------------------------------------------------------------

int ReloadComponent::GetClipAmmo() const
{
    return mClipAmmo;
}

//------------------------------------------------------------------------------------------------------------------------

int ReloadComponent::GetReserveAmmo() const
{
    return mReserveAmmo;
}

//------------------------------------------------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------------------------------------------------