#include "AstroidsPrivate.h"
#include "CollisionComponent.h"
#include "box2d/box2d.h"

CollisionComponent::CollisionComponent(GameObject * pOwner, GameManager & gameManager, b2World * pWorld, b2Body * pBody, sf::Vector2f size, bool isDynamic, bool isActive)
    : GameComponent(pOwner, gameManager)
    , mpWorld(pWorld)
    , mpBody(pBody)
    , mSize(size)
    , mIsActive(isActive)
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
    if (!mIsActive || !mpBody)
    {
        return;
    }

    GameObject * pOwner = GetGameManager().GetGameObject(mOwnerHandle);
    if (!pOwner || !pOwner->IsActive())
    {
        return;
    }

    float scale = pOwner->PIXELS_PER_METER;
    b2Vec2 bodyPos = mpBody->GetPosition();
    float angleRadians = mpBody->GetAngle();

    sf::Vector2f spritePos(bodyPos.x * scale, bodyPos.y * scale);
    float angleDegrees = angleRadians * (180.f / b2_pi);

    pOwner->SetPosition(spritePos);
    pOwner->SetRotation(angleDegrees);
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

void CollisionComponent::SetActive(bool active)
{
    mIsActive = active;
    if (mpBody)
    {
        for (b2Fixture * pFixture = mpBody->GetFixtureList(); pFixture; pFixture = pFixture->GetNext())
        {
            pFixture->SetSensor(!active); // true = sensor = no collision resolution
        }
    }
}

//------------------------------------------------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------------------------------------------------