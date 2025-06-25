#pragma once
#include "GameComponent.h"
#include <array>
#include "WeaponComponent.h"

class WeaponInventoryComponent : public GameComponent
{
public:
    WeaponInventoryComponent(GameObject * pOwner, GameManager & gameManager);
    ~WeaponInventoryComponent();

    virtual void Update(float deltaTime) override;
    virtual void draw(sf::RenderTarget & target, sf::RenderStates states) override;
    virtual void DebugImGuiComponentInfo() override;
    virtual const std::string & GetClassName() override;

    void AddWeapon(std::weak_ptr<WeaponComponent> weapon);
    void SwitchToSlot(int index);
    void SwitchToNextSlot();
    void Shoot();
    void Reload();

    bool IsReloading() const;
    int GetAmmoInCurrentClip() const;
    int GetActiveReserveAmmo() const;

    std::weak_ptr<WeaponComponent> GetActiveWeapon() const;
    void AddOrReplaceWeapon(EWeaponType weaponType);

private:
    bool TryAddWeapon(EWeaponType weaponType);
    void ReplaceCurrentWeapon(EWeaponType weaponType);

    void SetWeaponActiveState(int index, bool isActive);

    static constexpr int kMaxWeapons = 2;
    std::array<std::weak_ptr<WeaponComponent>, kMaxWeapons> mWeapons;
    int mCurrentIndex;
    std::string mName;
};

