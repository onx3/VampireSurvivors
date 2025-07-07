#pragma once
#include "GameComponent.h"
#include "WeaponManager.h"
enum class EBoxState
{
    Idle, 
    Flickering,
    WaitingToPickup
};

class MysteryBoxComponent : public GameComponent
{
public:
    MysteryBoxComponent(GameObject * pOwner, GameManager & gameManger);
    ~MysteryBoxComponent();

    virtual void Update(float deltaTime) override;
    virtual void draw(sf::RenderTarget & target, sf::RenderStates states) override;
    virtual void DebugImGuiComponentInfo() override;
    virtual const std::string & GetClassName() override;

    void StartCooldown();

    void SetDisplayWeaponTexture(std::shared_ptr<sf::Texture> texture);

private:
    sf::Sprite mDisplaySprite;
    std::shared_ptr<sf::Texture> mDisplayTexture;

    float mCoolDown;
    float mCurrentCooldown;
    float mInteractionRange;
    bool mIsPlayerInRange;
    bool mIsActive;
    int mPrice;
    sf::Text mInteractionText;
    sf::Font mFont;

    EBoxState mBoxState;
    float mFlickerTimer;
    float mFlickerDuration; // Total flicker time
    float mFlickerInterval; // Time between weapon changes
    float mTimeSinceLastFlicker;
    float mPickupWaitTimer;

    EWeaponType mCurrentFlickerWeapon = EWeaponType::Pistol;
    EWeaponType mFinalSelectedWeapon = EWeaponType::Pistol;
    std::string mName;
};

