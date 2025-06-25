#pragma once
#include "GameComponent.h"
#include <SFML/System/Vector2.hpp>
#include <vector>
#include <string>

struct Projectile
{
    BD::Handle handle;
    float lifespan;
    sf::Vector2f direction;
};

enum class EProjectileType
{
    RedLaser,
    GreenLaser
};

class ProjectileComponent : public GameComponent
{
public:
    ProjectileComponent(GameObject * pOwner, GameManager & gameManager);
    ~ProjectileComponent();

    std::string GetCorrectProjectileFile();

    void Shoot();

    virtual void Update(float deltaTime) override;
    virtual void DebugImGuiComponentInfo() override;
    virtual const std::string & GetClassName() override;

private:
    void UpdateProjectiles(float deltaTime);

    std::vector<Projectile> mProjectiles;
    float mSpeed;
    float mCooldown;
    float mTimeSinceLastShot;
    float mDamagePerShot;
    EProjectileType mLastUsedProjectile;
    
    //Sound
    sf::Sound mContinuousFireSound;
    bool mIsFiringSoundPlaying;
    bool mIsFadingOut;
    const float mStartVolume;
    const float mFadeRate;

    std::string mName;
};
