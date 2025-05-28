#include "AstroidsPrivate.h"
#include "RadiusPickupComponent.h"

RadiusPickupComponent::RadiusPickupComponent(GameObject * pOwner, GameManager & gameManager, const BD::Handle & followHandle, float radius)
    : GameComponent(pOwner, gameManager)
    , mFollowHandle(followHandle)
    , mRadius(radius)
    , mMoveSpeed(50.f)
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
    if (!pFollowObj || !pFollowObj->IsActive())
    {
        return;
    }

    auto & gameObj = GetGameObject();
    if (!gameObj.IsActive())
    {
        return;
    }

    sf::Vector2f myPos = gameObj.GetPosition();
    sf::Vector2f followPos = pFollowObj->GetPosition();
    sf::Vector2f directionVec = followPos - myPos;

    float distanceSqr = BD::GetMagnitudeSquared(directionVec);

    if (distanceSqr <= mRadius * mRadius || mStartedToTrack)
    {
        mStartedToTrack = true;

        float distance = std::sqrt(distanceSqr);
        if (distance > 0.0f)
        {
            sf::Vector2f direction = directionVec / distance;

            // Check for physics body
            if (b2Body * pBody = gameObj.GetPhysicsBody())
            {
                b2Vec2 velocity(
                    direction.x * mMoveSpeed / gameObj.PIXELS_PER_METER,
                    direction.y * mMoveSpeed / gameObj.PIXELS_PER_METER
                );
                pBody->SetLinearVelocity(velocity);
            }
            else
            {
                // Fallback: direct position update (non-physics)
                sf::Vector2f movement = direction * mMoveSpeed * deltaTime;
                gameObj.SetPosition(myPos + movement);
            }
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