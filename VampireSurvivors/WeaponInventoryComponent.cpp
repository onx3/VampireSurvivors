#include "AstroidsPrivate.h"
#include "WeaponInventoryComponent.h"
#include "FollowComponent.h"

WeaponInventoryComponent::WeaponInventoryComponent(GameObject * pOwner, GameManager & gameManager)
    : GameComponent(pOwner, gameManager)
    , mWeapons()
    , mCurrentIndex(0)
    , mName("WeaponInventoryComponent")
{

}

//------------------------------------------------------------------------------------------------------------------------

WeaponInventoryComponent::~WeaponInventoryComponent()
{

}

//------------------------------------------------------------------------------------------------------------------------

void WeaponInventoryComponent::Update(float deltaTime)
{
    if (auto pActiveWeapon = mWeapons[mCurrentIndex].lock())
    {
        pActiveWeapon->Update(deltaTime);
    }
}

//------------------------------------------------------------------------------------------------------------------------

void WeaponInventoryComponent::draw(sf::RenderTarget & target, sf::RenderStates states)
{

}

//------------------------------------------------------------------------------------------------------------------------

void WeaponInventoryComponent::DebugImGuiComponentInfo()
{

}

//------------------------------------------------------------------------------------------------------------------------

const std::string & WeaponInventoryComponent::GetClassName()
{
    return mName;
}

//------------------------------------------------------------------------------------------------------------------------

void WeaponInventoryComponent::AddWeapon(std::weak_ptr<WeaponComponent> weapon)
{
    for (int i = 0; i < kMaxWeapons; ++i)
    {
        if (mWeapons[i].expired())
        {
            mWeapons[i] = weapon;
            SetWeaponActiveState(i, i == mCurrentIndex);
            return;
        }
    }
}

//------------------------------------------------------------------------------------------------------------------------

void WeaponInventoryComponent::SwitchToSlot(int index)
{
    if (index < 0 || index >= kMaxWeapons || mWeapons[index].expired())
    {
        return;
    }

    SetWeaponActiveState(mCurrentIndex, false);
    mCurrentIndex = index;
    SetWeaponActiveState(mCurrentIndex, true);
}

//------------------------------------------------------------------------------------------------------------------------

void WeaponInventoryComponent::SwitchToNextSlot()
{
    SetWeaponActiveState(mCurrentIndex, false);
    mCurrentIndex = (mCurrentIndex == 1) ? 0 : 1;
    SetWeaponActiveState(mCurrentIndex, true);
}

//------------------------------------------------------------------------------------------------------------------------

void WeaponInventoryComponent::Shoot()
{
    if (auto pWeapon = mWeapons[mCurrentIndex].lock())
    {
        pWeapon->Shoot();
    }
}

//------------------------------------------------------------------------------------------------------------------------

void WeaponInventoryComponent::Reload()
{
    if (auto pWeapon = mWeapons[mCurrentIndex].lock())
    {
        pWeapon->StartReload();
    }
}

//------------------------------------------------------------------------------------------------------------------------

bool WeaponInventoryComponent::IsReloading() const
{
    if (auto pWeapon = mWeapons[mCurrentIndex].lock())
    {
        return pWeapon->IsReloading();
    }
    return false;
}

//------------------------------------------------------------------------------------------------------------------------

int WeaponInventoryComponent::GetAmmoInCurrentClip() const
{
    if (auto pWeapon = mWeapons[mCurrentIndex].lock())
    {
        return pWeapon->GetAmmoInClip();
    }
    return -1;
}

//------------------------------------------------------------------------------------------------------------------------

int WeaponInventoryComponent::GetActiveReserveAmmo() const
{
    if (auto pWeapon = mWeapons[mCurrentIndex].lock())
    {
        return pWeapon->GetReserveAmmo();
    }
    return -1;
}

//------------------------------------------------------------------------------------------------------------------------

std::weak_ptr<WeaponComponent> WeaponInventoryComponent::GetActiveWeapon() const
{
    return mWeapons[mCurrentIndex];
}

//------------------------------------------------------------------------------------------------------------------------

void WeaponInventoryComponent::AddOrReplaceWeapon(EWeaponType weaponType)
{
    if (!TryAddWeapon(weaponType))
    {
        ReplaceCurrentWeapon(weaponType);
    }
}

//------------------------------------------------------------------------------------------------------------------------

bool WeaponInventoryComponent::TryAddWeapon(EWeaponType weaponType)
{
    for (int i = 0; i < kMaxWeapons; ++i)
    {
        if (mWeapons[i].expired())
        {
            GameObject & gameObj = GetGameObject();
            GameManager & gameManager = GetGameManager();

            BD::Handle weaponHandle = gameManager.CreateNewGameObject(ETeam::FriendlyPersistant, gameObj.GetHandle());
            GameObject * pWeaponObj = gameManager.GetGameObject(weaponHandle);

            auto pNewWeapon = std::make_shared<WeaponComponent>(pWeaponObj, gameManager, weaponType);
            pWeaponObj->AddComponent(pNewWeapon);

            mWeapons[i] = pNewWeapon;
            SetWeaponActiveState(i, true);

            // Weapon Follow Component
            {
                auto pWeaponFollowComponent = pWeaponObj->GetComponent<FollowComponent>().lock();
                if (!pWeaponFollowComponent)
                {
                    auto pWeaponFollowComponent = std::make_shared<FollowComponent>(pWeaponObj, gameManager, gameObj.GetHandle(), sf::Vector2f(12, 10));
                    pWeaponObj->AddComponent(pWeaponFollowComponent);
                }
            }
            return true;
        }
    }
    return false;
}

//------------------------------------------------------------------------------------------------------------------------

void WeaponInventoryComponent::ReplaceCurrentWeapon(EWeaponType weaponType)
{
    auto pOldWeapon = mWeapons[mCurrentIndex].lock();
    if (pOldWeapon)
    {
        pOldWeapon->SetWeaponType(weaponType);
        SetWeaponActiveState(mCurrentIndex, true);
        return;
    }
}

//------------------------------------------------------------------------------------------------------------------------

void WeaponInventoryComponent::SetWeaponActiveState(int index, bool isActive)
{
    if (auto pWeapon = mWeapons[index].lock())
    {
        auto * pWeaponObj = &pWeapon->GetGameObject();
        pWeaponObj->SetIsActive(isActive);
    }
}

//------------------------------------------------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------------------------------------------------