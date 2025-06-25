#pragma once
#include "GameComponent.h"

struct Boomerang
{
	BD::Handle handle;
	float distanceSqr;
	float totalDistanceSqr;
	sf::Vector2f direction;
	bool isReturning = false;
};

class BoomerangComponent : public GameComponent
{
public:
	BoomerangComponent(GameObject * pOwner, GameManager & gameManager);
	~BoomerangComponent();

	virtual void Update(float deltaTime) override;
	virtual void DebugImGuiComponentInfo() override;
	virtual const std::string & GetClassName() override;

	void AddDamage(float damage);

private:
	void ThrowBoomerang(GameObject & enemy);
	void UpdateBoomerangs(float deltaTime);

	std::vector<Boomerang> mBoomerangs;
	float mTimeSinceLastThrow;
	float mCooldown;
	float mBoomerangDamagePerThrow;
	float mBoomerangDamageMult;
	float mSpeed;

	std::string mName;
};

