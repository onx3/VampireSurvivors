#pragma once
#include "GameComponent.h"
#include "ProjectileComponent.h"

class EnemyBulletComponent : public GameComponent
{
public:
    EnemyBulletComponent(GameObject * pOwner, GameManager & gameManager);
    ~EnemyBulletComponent();

    std::string GetCorrectProjectileFile();

    void Shoot();

    virtual void Update(float deltaTime) override;
    virtual void DebugImGuiComponentInfo() override;
    virtual std::string & GetClassName() override;

private:
    void UpdateProjectiles(float deltaTime);

    std::vector<Projectile> mBullets;
    float mSpeed;
    float mCooldown;
    float mTimeSinceLastShot;
    EProjectileType mLastUsedProjectile;
    std::string mName;
};

