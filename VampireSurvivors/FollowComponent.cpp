#include "AstroidsPrivate.h"
#include "FollowComponent.h"
#include "CameraManager.h"

FollowComponent::FollowComponent(GameObject * pOwner, GameManager & gameManager, BD::Handle followHandle, sf::Vector2f offset)
	: GameComponent(pOwner, gameManager)
	, mFollowHandle(followHandle)
	, mOffset(offset)
    , mOrbitRadius(0.f)
	, mName("FollowComponent")
{
    mOrbitRadius = std::sqrt(mOffset.x * mOffset.x + mOffset.y * mOffset.y);
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

    sf::Vector2f toCrosshair = crosshairPos - playerPos;
    float angleRad = std::atan2(toCrosshair.y, toCrosshair.x);

    // Orbit around the player
    sf::Vector2f orbitPos;
    orbitPos.x = playerPos.x + std::cos(angleRad) * mOrbitRadius;
    orbitPos.y = playerPos.y + std::sin(angleRad) * mOrbitRadius;
    GetGameObject().SetPosition(orbitPos);

    // Rotate object to face the crosshair
    sf::Vector2f aimDir = crosshairPos - orbitPos;
    float aimAngle = std::atan2(aimDir.y, aimDir.x) * 180.f / BD::gsPi;

    bool isFlipped = aimDir.x < 0.f;

    if (isFlipped)
    {
        aimAngle += 180.f;
    }

    GetGameObject().SetRotation(aimAngle);

    auto pSpriteComponent = GetGameObject().GetComponent<SpriteComponent>().lock();
    if (pSpriteComponent)
    {
        sf::Vector2f currentScale = pSpriteComponent->GetSprite().getScale();
        float xFlip = isFlipped ? -1.f : 1.f;

        if ((currentScale.x > 0.f && xFlip < 0.f) || (currentScale.x < 0.f && xFlip > 0.f))
        {
            currentScale.x *= -1.f;
            pSpriteComponent->SetScale(currentScale);
        }
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