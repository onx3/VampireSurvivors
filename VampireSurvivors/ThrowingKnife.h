#pragma once
#include "GameComponent.h"

struct ThrowingKnife
{
	BD::Handle handle;
	float lifespan;
	sf::Vector2f direction;
};

class ThrowingKnifeComponent : public GameComponent
{
public:
	ThrowingKnifeComponent(GameObject * pOwner, GameManager & gameManager);
	~ThrowingKnifeComponent();

	virtual void Update(float deltaTime) override;
	virtual void DebugImGuiComponentInfo() override;
	virtual const std::string & GetClassName() override;

	void AddDamage(float damage);

private:
	void ThrowKnife();
	void UpdateThrowingKnives(float deltaTime);

	std::vector<ThrowingKnife> mThrowingKnives;
	float mTimeSinceLastShot;
	float mCooldown;
	float mThrowingKnifeDamagePerShot;
	float mThrowingKnifeDamageMult;
	float mSpeed;

	std::string mName;
};

