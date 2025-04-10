#pragma once
#include "GameComponent.h"
class DamageComponent : public GameComponent
{
public:
	DamageComponent(GameObject * pOwner, GameManager & gameManager, int damage);
	~DamageComponent();

	virtual void Update(float deltaTime) override;
	virtual void DebugImGuiComponentInfo() override;
	virtual std::string & GetClassName() override;

	int GetDamageAmount();

private:
	int mDamage;
    std::string mName;
};

