#pragma once
#include "GameComponent.h"
class AISimplePathComponent : public GameComponent
{
public:
	AISimplePathComponent(GameObject * pOwner, GameManager & gameManager, BD::Handle followHandle);
	~AISimplePathComponent();

	virtual void Update(float deltaTime) override;
	virtual void DebugImGuiComponentInfo() override;
	virtual std::string & GetClassName() override;

private:
	BD::Handle mFollowHandle;
	float mMoveSpeed;
	std::string mName;
};

