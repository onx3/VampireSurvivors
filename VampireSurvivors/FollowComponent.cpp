#include "AstroidsPrivate.h"
#include "FollowComponent.h"

FollowComponent::FollowComponent(GameObject * pOwner, GameManager & gameManager, BD::Handle followHandle, sf::Vector2f offset)
	: GameComponent(pOwner, gameManager)
	, mFollowHandle(followHandle)
	, mOffset(offset)
	, mName("FollowComponent")
{

}

//------------------------------------------------------------------------------------------------------------------------

FollowComponent::~FollowComponent()
{

}

//------------------------------------------------------------------------------------------------------------------------

void FollowComponent::Update(float deltaTime)
{
	auto * pFollowObj = GetGameManager().GetGameObject(mFollowHandle);
	if (pFollowObj)
	{
		sf::Vector2f pos = pFollowObj->GetPosition() + mOffset;
		GetGameObject().SetPosition(pos);
	}
}

//------------------------------------------------------------------------------------------------------------------------

std::string & FollowComponent::GetClassName()
{
	return mName;
}

//------------------------------------------------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------------------------------------------------