#pragma once
#include "GameComponent.h"
enum class EAttackState
{
	Idle,
	WindUp,
	Cooldown
};

class EnemyMeleeAttackComponent : public GameComponent
{
public:
	EnemyMeleeAttackComponent(GameObject * pGameObject, GameManager & gameManager, BD::Handle & playerHandle);
	~EnemyMeleeAttackComponent();

	virtual void Update(float deltaTime) override;
	virtual void draw(sf::RenderTarget & target, sf::RenderStates states) override;
	virtual void DebugImGuiComponentInfo() override;
	virtual std::string & GetClassName() override;

private:
	EAttackState mAttackState;
	float mAttackRange;
	float mWindUpTime;
	float mAttackCooldown;
	float mTimer;
	float mDamage;
	BD::Handle mPlayerHandle;
	std::string mName;
};

