#pragma once
#include "GameComponent.h"
class DamageZoneComponent : public GameComponent
{
public:
    DamageZoneComponent(GameObject * pOwner, GameManager & gameManager, float damagePerSecond, float duration);

    virtual void Update(float deltaTime) override;
    virtual void DebugImGuiComponentInfo();
    virtual std::string & GetClassName();

private:
    float mDamagePerSecond;
    float mLifetime;
    float mDamageInterval; // damage every 1 second
    std::unordered_map<BD::Handle, float> mEnemyCooldowns;
    std::string mName;
};