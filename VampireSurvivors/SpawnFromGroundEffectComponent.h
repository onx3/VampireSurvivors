#pragma once
#include "GameComponent.h"
class SpawnFromGroundEffectComponent : public GameComponent
{
public:
    SpawnFromGroundEffectComponent(GameObject * pOwner, GameManager & gameManager, EEnemy enemyType, BD::Handle targetHandle, float health, float duration = 2.5f);

    virtual void Update(float deltaTime) override;
    virtual void DebugImGuiComponentInfo() override;
    virtual const std::string & GetClassName() override;

private:
    void AddLogicComponents();

    float mDuration;
    float mElapsed;
    float mHealth;
    bool mSpawned;
    EEnemy mEnemyType;
    BD::Handle mTargetHandle;
    std::string mName;
};

