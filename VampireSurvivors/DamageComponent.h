#pragma once
#include "GameComponent.h"
class DamageComponent : public GameComponent
{
public:
	DamageComponent(GameObject * pOwner, GameManager & gameManager, float damage);
	~DamageComponent();

	virtual void Update(float deltaTime) override;
	virtual void DebugImGuiComponentInfo() override;
	virtual std::string & GetClassName() override;

	float GetDamageAmount();

private:
	float mDamage;
    std::string mName;
};

