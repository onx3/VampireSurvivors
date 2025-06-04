#pragma once
#include "GameComponent.h"

enum class EWeaponType
{
    Pistol,
    Shotgun,
    SMG,
    Rifle,
    RPG
};

class WeaponComponent : public GameComponent
{
public:
    WeaponComponent(GameObject * pOwner, GameManager & gameManager, EWeaponType weaponType);
    ~WeaponComponent();

    virtual void Update(float deltaTime) override;
    virtual void draw(sf::RenderTarget & target, sf::RenderStates states) override;
    virtual void DebugImGuiComponentInfo() override;
    virtual std::string & GetClassName() override;

    void Shoot();
    void StartReload();

    int GetClipSize() const;
    int GetAmmoInClip() const;
    int GetReserveAmmo() const;
    EWeaponType GetWeaponType() const;
    bool IsReloading() const;
    bool CanShoot() const;
    

private:
    void ConsumeAmmo();

    EWeaponType mWeaponType;
    int mClipSize;
    int mClipAmmo;
    int mReserveAmmo;    // -1 = infinite
    float mReloadTime;
    float mReloadTimer;
    bool mIsReloading;
    float mFireCooldown;
    float mTimeBetweenShots;

    std::string mName;
};

