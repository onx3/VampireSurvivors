#pragma once
#include "GameComponent.h"

struct Slash
{
    BD::Handle handle;
    float lifespan;
    float damage;
};

class SwordSlashComponent : public GameComponent
{
public:
    SwordSlashComponent(GameObject * owner, GameManager & gameManagers, float arcAngleDeg, float radius, float duration);
    ~SwordSlashComponent();

    virtual void Update(float deltaTime) override;
    virtual void draw(sf::RenderTarget & target, sf::RenderStates states) override;
    virtual void DebugImGuiComponentInfo() override;
    virtual std::string & GetClassName() override;

    void SetRangeMultiplier(float mult);

private:
    void PerformSlash();

    void CleanUpSlashes(float deltaTime);

    void RebuildWedgeShape();

    std::vector<Slash> mSlashObjs;
    sf::ConvexShape mWedge;
    sf::Vector2f mSlashDirection;

    float mArcAngleRad;
    float mRadius;
    float mDuration;
    float mElapsedTime;
    float mTimeSinceLastSlash;
    float mCooldown;
    float mDamagePerSlash;
    float mRangeMult;
    bool mIsSlashing;
    std::string mName;
};

