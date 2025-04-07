#include "AstroidsPrivate.h"
#include "GameComponent.h"
#include "BDConfig.h"

GameComponent::GameComponent(GameObject * pOwner, GameManager & gameManager)
    : mOwnerHandle(pOwner->GetHandle())
    , mGameManager(gameManager)
    , mName("GameComponent")
{
}

//------------------------------------------------------------------------------------------------------------------------

void GameComponent::SetOwner(GameObject * pOwner)
{
    mOwnerHandle = pOwner->GetHandle();
}

//------------------------------------------------------------------------------------------------------------------------

GameObject & GameComponent::GetGameObject() const
{
    auto * pOwner = GetGameManager().GetGameObject(mOwnerHandle);
    assert(pOwner && "mpOwner is nullptr!");
    return *pOwner;
}

//------------------------------------------------------------------------------------------------------------------------

GameManager & GameComponent::GetGameManager() const
{
    return mGameManager;
}

//------------------------------------------------------------------------------------------------------------------------

void GameComponent::draw(sf::RenderTarget & target, sf::RenderStates states)
{

}

//------------------------------------------------------------------------------------------------------------------------

void GameComponent::DebugImGuiComponentInfo()
{
#if IMGUI_ENABLED()

#endif
}

//------------------------------------------------------------------------------------------------------------------------

std::string & GameComponent::GetClassName()
{
    return mName;
}

//------------------------------------------------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------------------------------------------------