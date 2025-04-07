#pragma once
#include "GameComponent.h"

class DropMovementComponent : public GameComponent
{
public:
	DropMovementComponent(GameObject * pGameOwner, GameManager & gameManager);

	virtual void Update(float deltaTime) override;
	virtual void DebugImGuiComponentInfo() override;
	virtual std::string & GetClassName() override;

private:
	sf::Vector2f mDirection;
	float mVelocity; 
	sf::Vector2f mStartPosition;
	std::string mName;
};

