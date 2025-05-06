#include "AstroidsPrivate.h"
#include "SpriteAnimationComponent.h"

SpriteAnimationComponent::SpriteAnimationComponent(GameObject * pOwner, GameManager & gameManager)
    : GameComponent(pOwner, gameManager)
    , mAnimations()
    , mCurrentAnimationName(EAnimationState::TOTAL)
    , mCurrentAnimation()
    , mCurrentFrame(0)
    , mTimer(0.f)
    , mName("SpriteAnimationComponent")
{
    mpSpriteComponent = pOwner->GetComponent<SpriteComponent>();
}

//------------------------------------------------------------------------------------------------------------------------

SpriteAnimationComponent::~SpriteAnimationComponent()
{

}

//------------------------------------------------------------------------------------------------------------------------

void SpriteAnimationComponent::Update(float deltaTime)
{
    auto pSpriteComponent = GetGameObject().GetComponent<SpriteComponent>().lock();
    if (!pSpriteComponent || mCurrentAnimation.frames.empty())
    {
        return;
    }

    mTimer += deltaTime;

    if (mTimer >= mCurrentAnimation.frameTime)
    {
        mTimer -= mCurrentAnimation.frameTime;
        mCurrentFrame = (mCurrentFrame + 1) % mCurrentAnimation.frames.size();
        pSpriteComponent->GetSprite().setTextureRect(mCurrentAnimation.frames[mCurrentFrame]);
    }
}

//------------------------------------------------------------------------------------------------------------------------

void SpriteAnimationComponent::DebugImGuiComponentInfo()
{

}

//------------------------------------------------------------------------------------------------------------------------

std::string & SpriteAnimationComponent::GetClassName()
{
    return mName;
}

//------------------------------------------------------------------------------------------------------------------------

void SpriteAnimationComponent::AddAnimation(EAnimationState state, const Animation & animation)
{
    mAnimations[state] = animation;
}

//------------------------------------------------------------------------------------------------------------------------

void SpriteAnimationComponent::PlayAnimation(EAnimationState state)
{
    if (mCurrentAnimationName != state && mAnimations.find(state) != mAnimations.end())
    {
        mCurrentAnimationName = state;
        mCurrentAnimation = mAnimations[state];
        mCurrentFrame = 0;
        mTimer = 0.f;

        auto pSpriteComponent = mpSpriteComponent.lock();
        if (pSpriteComponent && !mCurrentAnimation.frames.empty())
        {
            pSpriteComponent->GetSprite().setTextureRect(mCurrentAnimation.frames[0]);
        }
    }
}

//------------------------------------------------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------------------------------------------------