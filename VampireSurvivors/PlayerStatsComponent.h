#pragma once
#include "GameComponent.h"
class PlayerStatsComponent : public GameComponent
{
public:
	PlayerStatsComponent(GameObject * pOwner, GameManager & gameManager);
	~PlayerStatsComponent();

	virtual void Update(float deltaTime) override;
	virtual void DebugImGuiComponentInfo() override;
	virtual std::string & GetClassName() override;

	void AddAttackMult(float mult);
	void MultAttackRange(float rangeMult);

	float GetDamageMult() const;
	float GetRangeMult() const;

private:
	float mAttackMult;
	float mAttackRange;

	std::string mName;
};

