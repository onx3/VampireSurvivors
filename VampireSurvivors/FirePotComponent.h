#pragma once
#include "GameComponent.h"

struct FirePot
{
    BD::Handle firePotHandle;
    sf::Vector2f position;
    bool hasLanded;
    float timeLeft;
};

struct GhostSpriteFirePot
{
    sf::Vector2f position;
    float timeLeft;
};

class FirePotComponent : public GameComponent
{
public:
    FirePotComponent(GameObject * pOwner, GameManager & gameManager);

    virtual void Update(float deltaTime) override;
    virtual void DebugImGuiComponentInfo()override;
    virtual std::string & GetClassName()override;

private:

    void CastFirePot(GameObject & enemy);
    void CreateSpriteAnimationComponent(GameObject & pot);

    void UpdateFirePots(float deltaTime);

    std::vector<FirePot> mFirePots;
    std::unordered_map<BD::Handle, std::vector<GhostSpriteFirePot>> mGhostTrails;
    float mTimeSinceLastCast;
    float mCooldown;
    float mDamagePerSecond;
    float mDamageMult;
    float mSpeed;

    std::string mName;
};

