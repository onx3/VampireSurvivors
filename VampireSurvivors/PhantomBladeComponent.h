#pragma once
#include "GameComponent.h"

struct PhantomBlade
{
	BD::Handle phantomHandle;
	BD::Handle enemyHandle;
	float timeLeft;
};

struct GhostSprite
{
	sf::Vector2f position;
	float timeLeft;
};

class PhantomBladeComponent : public GameComponent
{
public:
	PhantomBladeComponent(GameObject * pOwner, GameManager & gameManager);
	~PhantomBladeComponent();

	virtual void Update(float deltaTime) override;
	virtual void draw(sf::RenderTarget & target, sf::RenderStates states);
	virtual void DebugImGuiComponentInfo() override;
	virtual std::string & GetClassName() override;

	void AddDamage(float damage);

private:
	void CastPhantomBlade(GameObject & enemy);
	void UpdatePhantomBlades(float deltaTime);

	void DrawGhostTrails(sf::RenderWindow & window);

	std::vector<PhantomBlade> mPhantomBlades;
	std::unordered_map<BD::Handle, std::vector<GhostSprite>> mGhostTrails;
	float mTimeSinceLastCast;
	float mCooldown;
	float mDamagePerSlash;
	float mDamageMult;
	float mSpeed;

	std::string mName;
	
};

