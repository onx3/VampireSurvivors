#pragma once
#include "GameComponent.h"
class PlayerShootingComponent : public GameComponent
{
public:
    PlayerShootingComponent(GameObject * pOwner, GameManager & gameManager);

    virtual void Update(float deltaTime) override;
    virtual void draw(sf::RenderTarget & target, sf::RenderStates states) override;
    virtual void DebugImGuiComponentInfo() override;
    virtual const std::string & GetClassName() override;

private:
    void Shoot();
    float mEmptyGunSoundCoolDown;
    float mEmptyGunSoundTimer;

    sf::Vector2f mLastTracerStart;
    sf::Vector2f mLastTracerEnd;
    float mTracerTimer;
    const float mTracerLifespan;
    std::string mName;
};

