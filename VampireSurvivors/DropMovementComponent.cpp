#include "AstroidsPrivate.h"
#include "DropMovementComponent.h"
#include "SpriteComponent.h"

DropMovementComponent::DropMovementComponent(GameObject * pGameOwner, GameManager & gameManager)
	: GameComponent(pGameOwner, gameManager)
    , mVelocity(100.f)
    , mName("DropMovementComponent")
{
    auto pSpriteComponent = GetGameObject().GetComponent<SpriteComponent>().lock();
    if (pSpriteComponent)
    {
        mStartPosition = pSpriteComponent->GetPosition();

        // Determine the movement direction based on the starting position
        sf::Vector2u windowSize = GetGameObject().GetGameManager().GetWindow().getSize();
        sf::Vector2f windowCenter(windowSize.x / 2.0f, windowSize.y / 2.0f);

        // Calculate direction from the starting position outward
        sf::Vector2f direction = mStartPosition - windowCenter;
        float magnitude = std::sqrt(direction.x * direction.x + direction.y * direction.y);
        if (magnitude != 0)
        {
            mDirection = direction / magnitude; // Normalize direction vector
        }
    }
}

//------------------------------------------------------------------------------------------------------------------------

void DropMovementComponent::Update(float deltaTime)
{
    GameObject * pOwner = GetGameManager().GetGameObject(mOwnerHandle);
    if (!pOwner || !pOwner->IsActive())
    {
        return;
    }

    auto spriteComponent = pOwner->GetComponent<SpriteComponent>().lock();
    if (!spriteComponent)
    {
        return;
    }

    auto & gameManager = pOwner->GetGameManager();
    sf::Vector2u windowSize = gameManager.GetWindow().getSize();

    // Move the object in the calculated direction
    sf::Vector2f position = spriteComponent->GetPosition();
    position += mDirection * mVelocity * deltaTime;

    spriteComponent->SetPosition(position);

    // Remove the object if it moves out of the screen
    if (position.x + spriteComponent->GetWidth() < 0 ||
        position.x > windowSize.x ||
        position.y + spriteComponent->GetHeight() < 0 ||
        position.y > windowSize.y)
    {
        pOwner->Destroy();
    }
}

//------------------------------------------------------------------------------------------------------------------------

void DropMovementComponent::DebugImGuiComponentInfo()
{
    
}

//------------------------------------------------------------------------------------------------------------------------

std::string & DropMovementComponent::GetClassName()
{
	return mName;
}

//------------------------------------------------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------------------------------------------------