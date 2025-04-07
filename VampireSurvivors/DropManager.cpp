#include "AstroidsPrivate.h"
#include "DropManager.h"
#include "SpriteComponent.h"
#include "CollisionComponent.h"
#include "ResourceManager.h"
#include "DropMovementComponent.h"
#include "ExplosionComponent.h"

DropManager::DropManager(GameManager * pGameManager)
	: BaseManager(pGameManager)
    , mDropHandles()
{

}

//------------------------------------------------------------------------------------------------------------------------

void DropManager::Update(float deltaTime)
{
    CleanUpDrops();

    GameManager & gameManager = GetGameManager();
    for (auto dropHandle : mDropHandles)
    {
        auto * pDrop = gameManager.GetGameObject(dropHandle);
        if (!pDrop->IsActive())
        {
            if (!pDrop->GetComponent<ExplosionComponent>().lock())
            {
                auto & window = gameManager.GetWindow();
                sf::Vector2u windowSize = window.getSize();
                sf::Vector2f centerPosition(float(windowSize.x) / 2.0f, float(windowSize.y) / 2.0f);
                auto explosionComp = std::make_shared<ExplosionComponent>(
                    pDrop, gameManager, "Art/explosion.png", 32, 32, 7, 0.1f, sf::Vector2f(50.f, 50.f), centerPosition);
                pDrop->AddComponent(explosionComp);
            }
        }
    }
}

//------------------------------------------------------------------------------------------------------------------------

void DropManager::OnGameEnd()
{
    mDropHandles.clear();
}

//------------------------------------------------------------------------------------------------------------------------

void DropManager::CleanUpDrops()
{
    auto & gameManager = GetGameManager();

    for (BD::Handle dropHandle : mDropHandles)
    {
        GameObject * pDrop = gameManager.GetGameObject(dropHandle);
        if (pDrop && !pDrop->IsDestroyed())
        {
            auto explosionComp = pDrop->GetComponent<ExplosionComponent>().lock();
            if (explosionComp && explosionComp->IsAnimationFinished())
            {
                pDrop->Destroy();
            }
        }
    }

    auto removeStart = std::remove_if(mDropHandles.begin(), mDropHandles.end(),
        [&gameManager](BD::Handle handle)
        {
            GameObject * pObj = gameManager.GetGameObject(handle);
            return pObj == nullptr || pObj->IsDestroyed();
        });

    mDropHandles.erase(removeStart, mDropHandles.end());
}

//------------------------------------------------------------------------------------------------------------------------

void DropManager::SpawnDrop(EDropType dropType, const sf::Vector2f & position)
{
    if (dropType == EDropType::None) return;

    auto & gameManager = GetGameManager();
    BD::Handle dropHandle;
    if (dropType == EDropType::NukePickup)
    {
        dropHandle = gameManager.CreateNewGameObject(ETeam::NukeDrop, gameManager.GetRootGameObjectHandle());
    }
    else if (dropType == EDropType::LifePickup)
    {
        dropHandle = gameManager.CreateNewGameObject(ETeam::LifeDrop, gameManager.GetRootGameObjectHandle());
    }
    mDropHandles.push_back(dropHandle);

    auto * pDrop = gameManager.GetGameObject(dropHandle);
    auto pSpriteComp = pDrop->GetComponent<SpriteComponent>().lock();

    if (pSpriteComp)
    {
        std::shared_ptr<sf::Texture> pSpriteTexture;
        std::string file;
        ResourceId resourceId("");

        switch (dropType)
        {
            case EDropType::NukePickup:
                file = "Art/Nuke.png";
                resourceId = ResourceId(file);
                pSpriteTexture = gameManager.GetManager<ResourceManager>()->GetTexture(resourceId);
                if (pSpriteTexture)
                {
                    pSpriteComp->SetSprite(pSpriteTexture, sf::Vector2f(1, 1));
                }
                break;
            case EDropType::LifePickup:
                file = "Art/Life.png";
                resourceId = ResourceId(file);
                pSpriteTexture = gameManager.GetManager<ResourceManager>()->GetTexture(resourceId);
                if (pSpriteTexture)
                {
                    pSpriteComp->SetSprite(pSpriteTexture, sf::Vector2f(1, 1));
                }
                break;
            default:
                return;
        }
        sf::Color greenTint(0, 255, 0, 255);
        pSpriteComp->GetSprite().setColor(greenTint);

        pSpriteComp->SetPosition(position);
        pDrop->AddComponent(pSpriteComp);

        auto pDropMovementComponent = std::make_shared<DropMovementComponent>(pDrop, gameManager);
        pDrop->AddComponent(pDropMovementComponent);

        // Add collision or interaction logic for pickup
        pDrop->CreatePhysicsBody(&gameManager.GetPhysicsWorld(), pDrop->GetSize(), true);

        auto pCollisionComp = std::make_shared<CollisionComponent>(
            pDrop, gameManager, &gameManager.GetPhysicsWorld(), pDrop->GetPhysicsBody(), pDrop->GetSize(), true);
        pDrop->AddComponent(pCollisionComp);
    }
}

//------------------------------------------------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------------------------------------------------