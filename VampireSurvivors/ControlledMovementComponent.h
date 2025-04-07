#pragma once

#include "GameComponent.h"
#include "DungeonManager.h"

enum class ESpriteTilt
{
	Normal,
	Right,
	Left
};

class ControlledMovementComponent : public GameComponent
{
public:
	ControlledMovementComponent(GameObject * pOwner, GameManager & gameManager);
	ControlledMovementComponent(GameObject * pOwner, GameManager & gameManager, float veloX, float veloY);

	~ControlledMovementComponent();

	virtual void Update(float deltaTime) override;
	virtual void DebugImGuiComponentInfo() override;
	virtual std::string & GetClassName() override;

	void SetVelocityX(float velo);
	void SetVelocityY(float velo);

private:
	sf::Vector2f mVelocity;
	float mVelocityX;
	float mVelocityY;
	float mAcceleration;
	float mDeceleration;
	float mMaxSpeed;
	std::string mName;
	ESpriteTilt mTilt;
};

//------------------------------------------------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------------------------------------------------