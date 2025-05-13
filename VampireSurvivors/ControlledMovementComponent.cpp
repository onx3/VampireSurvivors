#include "AstroidsPrivate.h"
#include "ControlledMovementComponent.h"
#include <cassert>
#include "GameObject.h"
#include "SpriteComponent.h"
#include "BDConfig.h"
#include "ResourceManager.h"
#include "CameraManager.h"
#include "LevelManager.h"
#include "imgui.h"
#include "SpriteAnimationComponent.h"

ControlledMovementComponent::ControlledMovementComponent(GameObject * pOwner, GameManager & gameManager)
    : GameComponent(pOwner, gameManager)
    , mVelocity(3.f, 3.f)
    , mAcceleration(800.f)
    , mDeceleration(1000.f)
    , mMaxSpeed(25.f)
    , mVelocityX(0.f)
    , mVelocityY(0.f)
    , mName("ControlledMovementComponent")
    , mTilt(ESpriteTilt::Normal)
{
}

//------------------------------------------------------------------------------------------------------------------------

ControlledMovementComponent::ControlledMovementComponent(GameObject * pOwner, GameManager & gameManager, float veloX, float veloY)
    : GameComponent(pOwner, gameManager)
    , mVelocity(3.f, 3.f)
    , mAcceleration(800.f)
    , mDeceleration(1000.f)
    , mMaxSpeed(35.f)
    , mVelocityX(veloX)
    , mVelocityY(veloY)
    , mTilt(ESpriteTilt::Normal)
{
}

//------------------------------------------------------------------------------------------------------------------------

ControlledMovementComponent::~ControlledMovementComponent()
{
}

//------------------------------------------------------------------------------------------------------------------------

void ControlledMovementComponent::Update(float deltaTime)
{
    GameObject * pOwner = GetGameManager().GetGameObject(mOwnerHandle);
    if (!pOwner || !pOwner->IsActive())
    {
        return;
    }

    b2Body * pBody = pOwner->GetPhysicsBody();
    if (!pBody)
    {
        return;
    }

    sf::Vector2f inputDirection = { 0.f, 0.f };

    // Input handling
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) 
    {
        inputDirection.y -= 1.f;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) 
    {
        inputDirection.y += 1.f;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
    {
        inputDirection.x -= 1.f;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
    {
        inputDirection.x += 1.f;
    }

    // Normalize to prevent diagonal speed boost
    if (inputDirection.x != 0.f || inputDirection.y != 0.f)
    {
        inputDirection /= std::sqrt(inputDirection.x * inputDirection.x + inputDirection.y * inputDirection.y);
    }

    // Apply acceleration
    mVelocity += inputDirection * mAcceleration * deltaTime;

    // Clamp velocity
    float velocityLength = std::hypot(mVelocity.x, mVelocity.y);
    if (velocityLength > mMaxSpeed)
    {
        mVelocity = (mVelocity / velocityLength) * mMaxSpeed;
    }

    // Apply deceleration if no input
    if (inputDirection.x == 0.f)
    {
        mVelocity.x -= std::min(std::abs(mVelocity.x), mDeceleration * deltaTime) * (mVelocity.x > 0.f ? 1.f : -1.f);
    }
    if (inputDirection.y == 0.f)
    {
        mVelocity.y -= std::min(std::abs(mVelocity.y), mDeceleration * deltaTime) * (mVelocity.y > 0.f ? 1.f : -1.f);
    }

    // Set velocity in physics engine
    if (mVelocity.x >= 5.f)
    {
        int ii = 0;
        ++ii;
    }
    b2Vec2 box2dVelocity(mVelocity.x / pOwner->PIXELS_PER_METER, mVelocity.y / pOwner->PIXELS_PER_METER);
    pBody->SetLinearVelocity(box2dVelocity);

    // Animation handling
    auto pAnimComponent = pOwner->GetComponent<SpriteAnimationComponent>().lock();
    if (pAnimComponent)
    {
        if (std::abs(mVelocity.x) > 0.1f || std::abs(mVelocity.y) > 0.1f)
        {
            pAnimComponent->PlayAnimation(EAnimationState::Move);
        }
        else
        {
            pAnimComponent->PlayAnimation(EAnimationState::Idle);
        }
    }

    // UV flip
    auto pSpriteComponent = pOwner->GetComponent<SpriteComponent>().lock();
    if (pSpriteComponent)
    {
        auto & sprite = pSpriteComponent->GetSprite();
        if (inputDirection.x > 0.f)
            sprite.setScale(std::abs(sprite.getScale().x), sprite.getScale().y);
        else if (inputDirection.x < 0.f)
            sprite.setScale(-std::abs(sprite.getScale().x), sprite.getScale().y);
    }
}

//------------------------------------------------------------------------------------------------------------------------

void ControlledMovementComponent::DebugImGuiComponentInfo()
{
    auto gameObjPos = GetGameObject().GetPosition();
    ImGui::Text("Position x,y: %.3f, %.3f", gameObjPos.x, gameObjPos.y);
}

//------------------------------------------------------------------------------------------------------------------------

std::string & ControlledMovementComponent::GetClassName()
{
    return mName;
}

//------------------------------------------------------------------------------------------------------------------------

void ControlledMovementComponent::SetVelocityX(float velo)
{
    mVelocityX = velo;
}

//------------------------------------------------------------------------------------------------------------------------

void ControlledMovementComponent::SetVelocityY(float velo)
{
    mVelocityY = velo;
}

//------------------------------------------------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------------------------------------------------