#include "AstroidsPrivate.h"
#include "FollowComponent.h"
#include "CameraManager.h"

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
    GameManager & gameManager = GetGameManager();
    auto * pFollowObj = gameManager.GetGameObject(mFollowHandle);
    if (!pFollowObj)
    {
        return;
    }

    auto * pCameraManager = gameManager.GetManager<CameraManager>();
    if (!pCameraManager)
    {
        return;
    }

    const sf::Vector2f & playerPos = pFollowObj->GetPosition();
    const sf::Vector2f & crosshairPos = pCameraManager->GetCrosshairPosition();

    // Vector from player to crosshair
    sf::Vector2f toCrosshair = crosshairPos - playerPos;
    float angleRad = std::atan2(toCrosshair.y, toCrosshair.x);

    // Use original offset length as orbit radius
    float radius = std::sqrt(mOffset.x * mOffset.x + mOffset.y * mOffset.y);

    // Calculate new object position on the circle around the player
    sf::Vector2f orbitPos;
    orbitPos.x = playerPos.x + std::cos(angleRad) * radius;
    orbitPos.y = playerPos.y + std::sin(angleRad) * radius;
    GetGameObject().SetPosition(orbitPos);

    // Rotate object to face the crosshair
    sf::Vector2f aimDir = crosshairPos - orbitPos;
    float aimAngle = std::atan2(aimDir.y, aimDir.x) * 180.f / BD::gsPi;
    GetGameObject().SetRotation(aimAngle);
}

//------------------------------------------------------------------------------------------------------------------------

std::string & FollowComponent::GetClassName()
{
	return mName;
}

//------------------------------------------------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------------------------------------------------