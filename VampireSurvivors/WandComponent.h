#pragma once
#include "GameComponent.h"

struct HomingShot
{
	BD::Handle selfHandle;
	BD::Handle enemyHandle;
	float speed;
};

class WandComponent : public GameComponent
{
public:
	WandComponent(GameObject * pOwner, GameManager & gameManager);
	~WandComponent();

	virtual void Update(float deltaTime) override;
	virtual void DebugImGuiComponentInfo() override;
	virtual std::string & GetClassName() override;

	void AddDamage(float damage);

private:
	void UpdateHomingShots(float deltaTime);

	void PerformHomingShot(GameObject & pEnemy);

	std::vector<HomingShot> mHomingShotObjs;
	float mTimeSinceLastShot;
	float mCooldown;
	float mDamagePerShot;
	float mDamageMult;
	float mSpeed;

	std::string mName;
};

