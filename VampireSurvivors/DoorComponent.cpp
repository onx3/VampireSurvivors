#include "AstroidsPrivate.h"
#include "DoorComponent.h"
#include "CollisionComponent.h"

DoorComponent::DoorComponent(GameObject * pOwner, GameManager & gameManager)
    : GameComponent(pOwner, gameManager)
    , mIsOpen(false)
    , mName("DoorComponent")
{
}

//------------------------------------------------------------------------------------------------------------------------

void DoorComponent::Update(float deltaTime)
{
}

//------------------------------------------------------------------------------------------------------------------------

void DoorComponent::DebugImGuiComponentInfo()
{
}

//------------------------------------------------------------------------------------------------------------------------

std::string & DoorComponent::GetClassName()
{
    return mName;
}

//------------------------------------------------------------------------------------------------------------------------

void DoorComponent::Open()
{
    if (mIsOpen)
    {
        return;
    }
    mIsOpen = true;
    auto pSpriteComp = GetGameObject().GetComponent<SpriteComponent>().lock();
    if (pSpriteComp)
    {
        ResourceId resId = ResourceId("../../VampireSurvivors/Art/Door/DoorOpen.png");
        auto pTexture = GetGameManager().GetManager<ResourceManager>()->GetTexture(resId);
        if (pTexture)
        {
            pSpriteComp->SetSprite(pTexture);
        }
    }
    auto pColisionComp = GetGameObject().GetComponent<CollisionComponent>().lock();
    if (pColisionComp)
    {
        pColisionComp->SetActive(false);
    }
}

//------------------------------------------------------------------------------------------------------------------------

void DoorComponent::Close()
{
    if (!mIsOpen)
    {
        return;
    }
    mIsOpen = false;
    auto pSpriteComp = GetGameObject().GetComponent<SpriteComponent>().lock();
    if (pSpriteComp)
    {
        ResourceId resId = ResourceId("../../VampireSurvivors/Art/Door/DoorClosed.png");
        auto pTexture = GetGameManager().GetManager<ResourceManager>()->GetTexture(resId);
        if (pTexture)
        {
            pSpriteComp->SetSprite(pTexture);
        }
    }
    auto pColisionComp = GetGameObject().GetComponent<CollisionComponent>().lock();
    if (pColisionComp)
    {
        pColisionComp->SetActive(true);
    }
}

//------------------------------------------------------------------------------------------------------------------------

bool DoorComponent::IsOpen()
{
    return mIsOpen;
}

//------------------------------------------------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------------------------------------------------