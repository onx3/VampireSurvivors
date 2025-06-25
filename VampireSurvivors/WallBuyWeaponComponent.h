#pragma once
#include "GameComponent.h"
#include "WeaponComponent.h"

class WallBuyWeaponComponent : public GameComponent
{
public:
    WallBuyWeaponComponent(GameObject * pOwner, GameManager & gameManager, const std::string & weaponType, int price);
    ~WallBuyWeaponComponent();

    virtual void Update(float deltaTime) override;
    virtual void draw(sf::RenderTarget & target, sf::RenderStates states) override;
    virtual void DebugImGuiComponentInfo() override;
    virtual const std::string & GetClassName() override;

private:
    void BuyWeapon(GameObject & playerObj);

    EWeaponType mWeaponType;
    int mPrice;
    float mInteractionRange;
    bool mIsPlayerInRange;
    sf::Text mInteractionText;
    sf::Font mFont;
    std::string mName;
};

