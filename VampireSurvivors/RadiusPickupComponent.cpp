#include "AstroidsPrivate.h"
#include "RadiusPickupComponent.h"

RadiusPickupComponent::RadiusPickupComponent(GameObject * pOwner, GameManager & gameManager, const BD::Handle & followHandle, float radius)
	: GameComponent(pOwner, gameManager)
	, mFollowHandle(followHandle)
	, mRadius(radius)
	, mMoveSpeed(200.f)
	, mStartedToTrack(false)
	, mName("RadiusPickupComponent")
{
}

//------------------------------------------------------------------------------------------------------------------------

RadiusPickupComponent::~RadiusPickupComponent()
{
}

//------------------------------------------------------------------------------------------------------------------------

void RadiusPickupComponent::Update(float deltaTime)
{
	auto & gameManager = GetGameManager();
	auto * pFollowObj = gameManager.GetGameObject(mFollowHandle);
	if (!pFollowObj)
	{
		return;
	}

	auto & gameObj = GetGameObject();

	sf::Vector2f myPos = gameObj.GetPosition();
	sf::Vector2f followPos = pFollowObj->GetPosition();
	sf::Vector2f directionVec = followPos - myPos;

	auto distanceSqr = BD::GetMagnitudeSquared(directionVec);
	if ((distanceSqr <= mRadius * mRadius) || mStartedToTrack)
	{
		mStartedToTrack = true;
		float length = std::sqrt(distanceSqr);
		if (length > 0.0f)
		{
			sf::Vector2f direction = directionVec / length;
			sf::Vector2f movement = direction * mMoveSpeed * deltaTime;

			gameObj.SetPosition(myPos + movement);
		}
	}
}

//------------------------------------------------------------------------------------------------------------------------

void RadiusPickupComponent::DebugImGuiComponentInfo()
{

}

//------------------------------------------------------------------------------------------------------------------------

std::string & RadiusPickupComponent::GetClassName()
{
	return mName;
}

//------------------------------------------------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------------------------------------------------