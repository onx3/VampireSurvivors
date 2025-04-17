#pragma once
#include "GameComponent.h"

struct HomingShot
{
	BD::Handle selfHandle;
	BD::Handle enemyHandle;
	float damage;
	float speed;
};

class WandComponent : public GameComponent
{
public:
	WandComponent(GameObject * pOwner, GameManager & gameManager);
	~WandComponent();

	virtual void Update(float deltaTime) override;
	virtual void draw(sf::RenderTarget & target, sf::RenderStates states) override;
	virtual void DebugImGuiComponentInfo() override;
	virtual std::string & GetClassName() override;

	void AddDamage(float damage);

private:
	GameObject * FindClosestEnemy();
	void UpdateHomingShots(float deltaTime);

	void PerformHomingShot(GameObject * pEnemy);

	std::vector<HomingShot> mHomingShotObjs;
	float mElapsedTime;
	float mTimeSinceLastShot;
	float mCooldown;
	float mDamagePerShot;
	float mDamageMult;
	float mSpeed;

	std::string mName;
};

