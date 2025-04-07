#pragma once
#include "GameComponent.h"
class TrackingComponent : public GameComponent
{
public:
	TrackingComponent(GameObject * pOwner, GameManager & gameManager, BD::Handle trackedHandle);
	~TrackingComponent();

	virtual void Update(float deltaTime) override;
	virtual std::string & GetClassName() override;

private:
	BD::Handle mTracker;
	std::string mName;
};

