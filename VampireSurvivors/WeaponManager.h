#pragma once
#include "BaseManager.h"

enum class EWeaponType
{
    Pistol,
    Shotgun,
    SMG,
    Rifle,
    RPG,

    Count
};

struct WeaponInfo
{
    EWeaponType weaponType;
    std::string name;
    std::string spritePath;
    int price;
};

class WeaponManager : public BaseManager
{
public:
    WeaponManager(GameManager * gameManager);
    ~WeaponManager();

    const WeaponInfo * GetWeaponInfo(EWeaponType type) const;
    bool GiveWeaponToPlayer(GameObject & player, EWeaponType type);
    const std::vector<EWeaponType> & GetAvailableWeapons() const;

    virtual void Update(float deltaTime) override;
    virtual void Render(sf::RenderWindow & window) override;
    virtual void OnGameEnd() override;

private:
    void InitializeWeapons();

    std::unordered_map<EWeaponType, WeaponInfo> mWeaponInfos;
    std::vector<EWeaponType> mAvailableWeapons;
};

