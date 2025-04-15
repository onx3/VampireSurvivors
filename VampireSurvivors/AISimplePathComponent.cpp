#include "AstroidsPrivate.h"
#include "AISimplePathComponent.h"

AISimplePathComponent::AISimplePathComponent(GameObject * pOwner, GameManager & gameManager, BD::Handle followHandle)
	: GameComponent(pOwner, gameManager)
	, mFollowHandle(followHandle)
	, mName("AISimplePathComponent")
	, mMoveSpeed(100.f)
{

}

//------------------------------------------------------------------------------------------------------------------------

AISimplePathComponent::~AISimplePathComponent()
{

}

//------------------------------------------------------------------------------------------------------------------------

void AISimplePathComponent::Update(float deltaTime)
{
	GameObject * pFollowObj = GetGameManager().GetGameObject(mFollowHandle);
	if (!pFollowObj)
	{
		return;
	}
	GameObject & myGameObj = GetGameObject();
	auto myPos = myGameObj.GetPosition();
	auto followPos = pFollowObj->GetPosition();

	sf::Vector2f direction = followPos - myPos;
	float distance = std::sqrt(direction.x * direction.x + direction.y * direction.y);

	if (distance > 0.f)
	{
		direction /= distance;
		sf::Vector2f movement = direction * mMoveSpeed * deltaTime;
		myGameObj.SetPosition(myPos + movement);
	}
}

//------------------------------------------------------------------------------------------------------------------------

void AISimplePathComponent::DebugImGuiComponentInfo()
{

}

//------------------------------------------------------------------------------------------------------------------------

std::string & AISimplePathComponent::GetClassName()
{
	return mName;
}

//------------------------------------------------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------------------------------------------------