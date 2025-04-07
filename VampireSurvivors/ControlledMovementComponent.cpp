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

ControlledMovementComponent::ControlledMovementComponent(GameObject * pOwner, GameManager & gameManager)
    : GameComponent(pOwner, gameManager)
    , mVelocity(3.f, 3.f)
    , mAcceleration(800.f)
    , mDeceleration(1000.f)
    , mMaxSpeed(300.f)
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
    , mMaxSpeed(300.f)
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

    auto pSpriteComponent = GetGameObject().GetComponent<SpriteComponent>().lock();

    if (pSpriteComponent)
    {
        // Get current position, size, and window bounds
        auto position = pSpriteComponent->GetPosition();
        sf::Vector2f size(pSpriteComponent->GetWidth(), pSpriteComponent->GetHeight());
        sf::Vector2u windowSize = GetGameObject().GetGameManager().GetWindow().getSize();

        sf::Vector2f inputDirection = { 0.f, 0.f };

        // Input handling
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) inputDirection.y -= 1.f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) inputDirection.y += 1.f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) inputDirection.x -= 1.f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) inputDirection.x += 1.f;

        // Normalize input direction to prevent faster diagonal movement
        if (inputDirection.x != 0.f || inputDirection.y != 0.f)
        {
            inputDirection /= std::sqrt(inputDirection.x * inputDirection.x + inputDirection.y * inputDirection.y);
        }

        // Apply acceleration
        mVelocity += inputDirection * mAcceleration * deltaTime;

        // Clamp velocity to max speed
        float velocityLength = std::hypot(mVelocity.x, mVelocity.y);
        if (velocityLength > mMaxSpeed)
        {
            mVelocity = (mVelocity / velocityLength) * mMaxSpeed;
        }

        // Apply deceleration if no input
        if (inputDirection.x == 0)
        {
            mVelocity.x -= std::min(std::abs(mVelocity.x), mDeceleration * deltaTime) * (mVelocity.x > 0 ? 1 : -1);
        }
        if (inputDirection.y == 0)
        {
            mVelocity.y -= std::min(std::abs(mVelocity.y), mDeceleration * deltaTime) * (mVelocity.y > 0 ? 1 : -1);
        }

        // Calculate new position
        sf::Vector2f newPosition = position + mVelocity * deltaTime;

        // Get grid and tile size
        auto & gameManager = GetGameObject().GetGameManager();
        auto pLevelManager = gameManager.GetManager<LevelManager>();
        float cellSize = BD::gsPixelCountCellSize;

        // Check if the tile is walkable
        {
             if (pLevelManager)
             {
                 // Determine the tile under the new position
                 int tileX = static_cast<int>(newPosition.x / cellSize);
                 int tileY = static_cast<int>(newPosition.y / cellSize);

                 if (pLevelManager->IsTileWalkablePlayer(tileX, tileY))
                 {
                     position = newPosition;
                 }
             }
        }
        pSpriteComponent->SetPosition(position);

        auto crosshairPosition = GetGameObject().GetGameManager().GetManager<CameraManager>()->GetCrosshairPosition();
        sf::Vector2f direction = crosshairPosition - position;
        float angle = std::atan2(direction.y, direction.x) * 180.f / 3.14159f;
        pSpriteComponent->SetRotation(angle + 90.f);
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