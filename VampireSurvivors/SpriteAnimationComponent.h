#pragma once
#include "GameComponent.h"

enum class EAnimationState
{
    Idle,
    Move,
    Attack,
    Death,
    TOTAL
};

//------------------------------------------------------------------------------------------------------------------------

struct Animation
{
    std::vector<sf::IntRect> frames;
    float frameTime = .1f;
};

//------------------------------------------------------------------------------------------------------------------------

class SpriteAnimationComponent : public GameComponent
{
public:
    SpriteAnimationComponent(GameObject * pOwner, GameManager & gameManager);
    ~SpriteAnimationComponent();

    virtual void Update(float deltaTime) override;
    virtual void DebugImGuiComponentInfo() override;
    virtual std::string & GetClassName() override;

    void AddAnimation(EAnimationState state, const Animation & animation);
    void PlayAnimation(EAnimationState state);

private:
    std::unordered_map<EAnimationState, Animation> mAnimations;
    EAnimationState mCurrentAnimationName;
    Animation mCurrentAnimation;
    int mCurrentFrame;
    float mTimer;
    std::string mName;

    std::weak_ptr<SpriteComponent> mpSpriteComponent;
};

