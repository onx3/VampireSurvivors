#pragma once
#include "GameComponent.h"
class RadiusPickupComponent : public GameComponent
{
public:
	RadiusPickupComponent(GameObject * pOwner, GameManager & gameManager, const BD::Handle & followHandle, float radius);
	~RadiusPickupComponent();

	virtual void Update(float deltaTime) override;
	virtual void DebugImGuiComponentInfo() override;
	virtual const std::string & GetClassName() override;

private:
	BD::Handle mFollowHandle;
	float mRadius;
	float mMoveSpeed;
	bool mStartedToTrack; // Use this so it doesn't stop once started
	std::string mName;
};

