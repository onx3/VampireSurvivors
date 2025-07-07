#include "AstroidsPrivate.h"
#include "WeaponManager.h"
#include "WeaponInventoryComponent.h"

WeaponManager::WeaponManager(GameManager * gameManager)
    : BaseManager(gameManager)
{
    InitializeWeapons();
}

//------------------------------------------------------------------------------------------------------------------------

WeaponManager::~WeaponManager()
{
}

//------------------------------------------------------------------------------------------------------------------------

void WeaponManager::InitializeWeapons()
{
    auto addWeapon = [&](EWeaponType type, const std::string & name, const std::string & path, int price)
        {
            mAvailableWeapons.push_back(type);
            mWeaponInfos[type] = { type, name, path, price };
        };
    addWeapon(EWeaponType::Pistol, "Pistol", "../../VampireSurvivors/Art/Weapons/Guns/Glock - P80 [64x48].png", 0);
    addWeapon(EWeaponType::Shotgun, "Shotgun", "../../VampireSurvivors/Art/Weapons/Guns/[32x96]Shotgun_V1.01.png", 0);
    addWeapon(EWeaponType::SMG, "SMG", "../../VampireSurvivors/Art/Weapons/Guns/Submachine - MP5A3 [80x48].png", 0);
    addWeapon(EWeaponType::Rifle, "Rifle", "../../VampireSurvivors/Art/Weapons/Guns/AK 47 [96x48].png", 0);
    addWeapon(EWeaponType::RPG, "RPG", "../../VampireSurvivors/Art/Weapons/Guns/Thick Bazooka - M20 [192x32].png", 0);
}

//------------------------------------------------------------------------------------------------------------------------

const WeaponInfo * WeaponManager::GetWeaponInfo(EWeaponType type) const
{
    auto it = mWeaponInfos.find(type);
    return (it != mWeaponInfos.end()) ? &it->second : nullptr;
}

//------------------------------------------------------------------------------------------------------------------------

bool WeaponManager::GiveWeaponToPlayer(GameObject & player, EWeaponType type)
{
    auto pInventory = player.GetComponent<WeaponInventoryComponent>().lock();
    if (!pInventory)
    {
        return false;
    }

    pInventory->AddOrReplaceWeapon(type);
    return true;
}

//------------------------------------------------------------------------------------------------------------------------

const std::vector<EWeaponType> & WeaponManager::GetAvailableWeapons() const
{
    return mAvailableWeapons;
}

//------------------------------------------------------------------------------------------------------------------------

void WeaponManager::Update(float deltaTime)
{
}

//------------------------------------------------------------------------------------------------------------------------

void WeaponManager::Render(sf::RenderWindow & window)
{
}

//------------------------------------------------------------------------------------------------------------------------

void WeaponManager::OnGameEnd()
{
}

//------------------------------------------------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------------------------------------------------