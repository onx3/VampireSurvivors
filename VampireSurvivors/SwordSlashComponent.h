#pragma once
#include "GameComponent.h"
class SwordSlashComponent : public GameComponent
{
public:
    SwordSlashComponent(GameObject * owner, GameManager & gameManagers, float arcAngleDeg, float radius, float duration);
    ~SwordSlashComponent();

    virtual void Update(float deltaTime) override;
    virtual void draw(sf::RenderTarget & target, sf::RenderStates states) override;
    virtual void DebugImGuiComponentInfo() override;
    virtual std::string & GetClassName() override;


private:
    void PerformSlash(const sf::Vector2f & direction);

    sf::Vector2f mCenter;
    sf::ConvexShape mWedge;

    float mArcAngleRad;
    float mRadius;
    float mDuration;
    float mElapsedTime;
    float mTimeSinceLastSlash;
    float mCooldown;

    bool mIsSlashing;
    b2Fixture * mpFixture;

    std::string mName;
};

