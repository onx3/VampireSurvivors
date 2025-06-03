#pragma once
#include "GameComponent.h"
class ReloadComponent : public GameComponent
{
public:
    ReloadComponent(GameObject * pOwner, GameManager & gameManger, int clipSize = 10, int reserveAmmo = 30, float reloadTime = 2.f);
    ~ReloadComponent();

    virtual void Update(float deltaTime) override;
    virtual void draw(sf::RenderTarget & target, sf::RenderStates states) override;
    virtual void DebugImGuiComponentInfo() override;
    virtual std::string & GetClassName() override;

    void ConsumeAmmo();
    void StartReload();

    bool CanShoot() const;
    bool IsReloading() const;
    int GetClipAmmo() const;
    int GetReserveAmmo() const;

private:
    int mClipAmmo;
    int mClipSize;
    int mReserveAmmo;
    float mReloadTime;
    float mReloadTimer;
    bool mIsReloading;
    std::string mName;
};

