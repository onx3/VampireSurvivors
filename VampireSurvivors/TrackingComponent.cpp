#include "AstroidsPrivate.h"
#include "TrackingComponent.h"

TrackingComponent::TrackingComponent(GameObject * pOwner, GameManager & gameManager, BD::Handle trackedHandle)
	: GameComponent(pOwner, gameManager)
	, mTracker(trackedHandle)
    , mName("TrackingComponent")
{

}

//------------------------------------------------------------------------------------------------------------------------

TrackingComponent::~TrackingComponent()
{

}

//------------------------------------------------------------------------------------------------------------------------

void TrackingComponent::Update(float deltaTime)
{
    GameManager & gameManager = GetGameManager();

    GameObject * pGameObject = gameManager.GetGameObject(mOwnerHandle);
    GameObject * pTrackedGameObject = gameManager.GetGameObject(mTracker);

    if (!pGameObject || !pTrackedGameObject)
    {
        return;
    }

    sf::Vector2f ownerPosition = pGameObject->GetPosition();
    sf::Vector2f trackedPosition = pTrackedGameObject->GetPosition();

    sf::Vector2f direction = trackedPosition - ownerPosition;
    float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
    if (length != 0)
    {
        direction /= length;
    }

    float angleDegrees = std::atan2(direction.y, direction.x) * (180.f / 3.14159265f);

    pGameObject->SetRotation(angleDegrees + 90.f);
}

//------------------------------------------------------------------------------------------------------------------------

std::string & TrackingComponent::GetClassName()
{
    return mName;
}

//------------------------------------------------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------------------------------------------------