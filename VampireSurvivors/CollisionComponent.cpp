#include "AstroidsPrivate.h"
#include "CollisionComponent.h"
#include "box2d/box2d.h"

CollisionComponent::CollisionComponent(GameObject * pOwner, GameManager & gameManager, b2World * pWorld, b2Body * pBody, sf::Vector2f size, bool isDynamic)
    : GameComponent(pOwner, gameManager)
    , mpWorld(pWorld)
    , mpBody(pBody)
    , mSize(size)
    , mName("CollisionComponent")
{
    mpBody->SetSleepingAllowed(false);
}

//------------------------------------------------------------------------------------------------------------------------

CollisionComponent::~CollisionComponent()
{
    mpWorld->DestroyBody(mpBody);
}

//------------------------------------------------------------------------------------------------------------------------

void CollisionComponent::Update(float deltaTime)
{
    GameObject * pOwner = GetGameManager().GetGameObject(mOwnerHandle);
    if (!pOwner || !pOwner->IsActive() || !mpBody)
    {
        return;
    }

    float scale = pOwner->PIXELS_PER_METER;
    auto spritePos = pOwner->GetPosition();
    b2Vec2 box2dPosition(spritePos.x / scale, spritePos.y / scale);
    auto rotation = pOwner->GetRotationDegrees() * (b2_pi / 180.0f);

    // Only update if there's a difference
    if (box2dPosition != mpBody->GetPosition() || rotation != mpBody->GetAngle())
    {
        mpBody->SetTransform(box2dPosition, rotation);
    }
}

//------------------------------------------------------------------------------------------------------------------------

void CollisionComponent::DebugImGuiComponentInfo()
{

}

//------------------------------------------------------------------------------------------------------------------------

std::string & CollisionComponent::GetClassName()
{
    return mName;
}

//------------------------------------------------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------------------------------------------------