#pragma once
#include "GameComponent.h"
#include "WeaponManager.h"

class WeaponComponent : public GameComponent
{
public:
    WeaponComponent(GameObject * pOwner, GameManager & gameManager, EWeaponType weaponType);
    ~WeaponComponent();

    virtual void Update(float deltaTime) override;
    virtual void draw(sf::RenderTarget & target, sf::RenderStates states) override;
    virtual void DebugImGuiComponentInfo() override;
    virtual const std::string & GetClassName() override;

    void Shoot();
    void StartReload();

    int GetClipSize() const;
    int GetAmmoInClip() const;
    int GetReserveAmmo() const;
    EWeaponType GetWeaponType() const;
    bool IsReloading() const;
    bool CanShoot() const;
    
    static EWeaponType ConvertStringToWeaponType(const std::string & string);

    void SetWeaponType(EWeaponType newType);

private:
    void SetUpSprite();
    void SetUpWeapon();
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

