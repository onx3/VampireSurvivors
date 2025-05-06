#pragma once
#include "GameComponent.h"
#include "unordered_set"

class DamageZoneComponent : public GameComponent
{
public:
    DamageZoneComponent(GameObject * pOwner, GameManager & gameManager, float damagePerSecond, float duration);

    virtual void Update(float deltaTime) override;
    virtual void DebugImGuiComponentInfo();
    virtual std::string & GetClassName();

    void OnBeginOverlap(BD::Handle enemyHandle);
    void OnEndOverlap(BD::Handle enemyHandle);


private:
    float mDamagePerSecond;
    float mLifetime;
    float mDamageInterval; // damage every 1 second
    std::unordered_map<BD::Handle, float> mEnemyCooldowns;
    std::unordered_set<BD::Handle> mEnemiesInside;
    std::string mName;
};