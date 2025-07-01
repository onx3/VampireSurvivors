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

    float scale = BD::gsPixelsPerMeter;
    b2Vec2 bodyPos = mpBody->GetPosition();
    float angleRadians = mpBody->GetAngle();

    sf::Vector2f spritePos(bodyPos.x * scale, bodyPos.y * scale);
    float angleDegrees = angleRadians * (180.f / b2_pi);

    pOwner->SetPosition(spritePos);
    //pOwner->SetRotation(angleDegrees); //Dont think I need this but leaving in case
}

//------------------------------------------------------------------------------------------------------------------------

void CollisionComponent::DebugImGuiComponentInfo()
{

}

//------------------------------------------------------------------------------------------------------------------------

const std::string & CollisionComponent::GetClassName()
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

bool CollisionComponent::IntersectsLine(const sf::Vector2f & a, const sf::Vector2f & b) const
{
    const sf::FloatRect bounds = GetWorldAABB(); // Or however you get the object's world-space bounds
    return BD::LineIntersectsAABB(a, b, bounds);
}

//------------------------------------------------------------------------------------------------------------------------

sf::FloatRect CollisionComponent::GetWorldAABB() const
{
    if (!mpBody)
    {
        return sf::FloatRect();
    }

    float pixelsPerMeter = BD::gsPixelsPerMeter;

    sf::FloatRect aabb;
    aabb.left = FLT_MAX;
    aabb.top = FLT_MAX;
    float right = -FLT_MAX;
    float bottom = -FLT_MAX;

    for (b2Fixture * fixture = mpBody->GetFixtureList(); fixture; fixture = fixture->GetNext())
    {
        const b2Shape * shape = fixture->GetShape();
        if (!shape)
            continue;

        b2AABB box;
        shape->ComputeAABB(&box, mpBody->GetTransform(), 0);

        float left = box.lowerBound.x * pixelsPerMeter;
        float top = box.lowerBound.y * pixelsPerMeter;
        float w = (box.upperBound.x - box.lowerBound.x) * pixelsPerMeter;
        float h = (box.upperBound.y - box.lowerBound.y) * pixelsPerMeter;

        aabb.left = std::min(aabb.left, left);
        aabb.top = std::min(aabb.top, top);
        right = std::max(right, left + w);
        bottom = std::max(bottom, top + h);
    }

    aabb.width = right - aabb.left;
    aabb.height = bottom - aabb.top;

    return aabb;
}

//------------------------------------------------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------------------------------------------------