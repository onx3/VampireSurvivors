#include "AstroidsPrivate.h"
#include "DropManager.h"
#include "SpriteComponent.h"
#include "CollisionComponent.h"
#include "ResourceManager.h"
#include "DropMovementComponent.h"
#include "ExplosionComponent.h"
#include "RadiusPickupComponent.h"
#include "PlayerManager.h"
#include "SpriteAnimationComponent.h"
#include "LightComponent.h"

DropManager::DropManager(GameManager * pGameManager)
	: BaseManager(pGameManager)
    , mRadius(75.f)
    , mDropHandles()
{

}

//------------------------------------------------------------------------------------------------------------------------

void DropManager::Update(float deltaTime)
{
    CleanUpDrops();
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
        if (pDrop && !pDrop->IsActive())
        {
            pDrop->Destroy();
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

//void DropManager::SpawnDrop(EDropType dropType, const sf::Vector2f & position)
//{
//    if (dropType == EDropType::None)
//    {
//        return;
//    }
//
//    auto & gameManager = GetGameManager();
//    BD::Handle dropHandle;
//    if (dropType == EDropType::NukePickup)
//    {
//        dropHandle = gameManager.CreateNewGameObject(ETeam::NukeDrop, gameManager.GetRootGameObjectHandle());
//    }
//    else if (dropType == EDropType::LifePickup)
//    {
//        dropHandle = gameManager.CreateNewGameObject(ETeam::LifeDrop, gameManager.GetRootGameObjectHandle());
//    }
//    mDropHandles.push_back(dropHandle);
//
//    auto * pDrop = gameManager.GetGameObject(dropHandle);
//    auto pSpriteComp = pDrop->GetComponent<SpriteComponent>().lock();
//
//    if (pSpriteComp)
//    {
//        std::shared_ptr<sf::Texture> pSpriteTexture;
//        std::string file;
//        ResourceId resourceId("");
//
//        switch (dropType)
//        {
//            case EDropType::NukePickup:
//                file = "../../VampireSurvivors/Art/Nuke.png";
//                resourceId = ResourceId(file);
//                pSpriteTexture = gameManager.GetManager<ResourceManager>()->GetTexture(resourceId);
//                if (pSpriteTexture)
//                {
//                    pSpriteComp->SetSprite(pSpriteTexture, sf::Vector2f(1, 1));
//                }
//                break;
//            case EDropType::LifePickup:
//                file = "../../VampireSurvivors/Art/Life.png";
//                resourceId = ResourceId(file);
//                pSpriteTexture = gameManager.GetManager<ResourceManager>()->GetTexture(resourceId);
//                if (pSpriteTexture)
//                {
//                    pSpriteComp->SetSprite(pSpriteTexture, sf::Vector2f(1, 1));
//                }
//                break;
//            default:
//                return;
//        }
//        sf::Color greenTint(0, 255, 0, 255);
//        pSpriteComp->GetSprite().setColor(greenTint);
//
//        pSpriteComp->SetPosition(position);
//        pDrop->AddComponent(pSpriteComp);
//
//        auto pDropMovementComponent = std::make_shared<DropMovementComponent>(pDrop, gameManager);
//        pDrop->AddComponent(pDropMovementComponent);
//
//        // Add collision or interaction logic for pickup
//        pDrop->CreateBoxShapePhysicsBody(&gameManager.GetPhysicsWorld(), pDrop->GetSize(), true);
//
//        auto pCollisionComp = std::make_shared<CollisionComponent>(
//            pDrop, gameManager, &gameManager.GetPhysicsWorld(), pDrop->GetPhysicsBody(), pDrop->GetSize(), true);
//        pDrop->AddComponent(pCollisionComp);
//    }
//}

//------------------------------------------------------------------------------------------------------------------------

void DropManager::DropCoins(const sf::Vector2f & position)
{
    auto & gameManager = GetGameManager();

    BD::Handle coinDropHandle = gameManager.CreateNewGameObject(ETeam::CoinDrop, gameManager.GetRootGameObjectHandle());
    mDropHandles.push_back(coinDropHandle);

    auto * pCoinDrop = gameManager.GetGameObject(coinDropHandle);
    if (!pCoinDrop)
    {
        return;
    }

    auto pSpriteComp = pCoinDrop->GetComponent<SpriteComponent>().lock();
    if (pSpriteComp)
    {
        std::string file = "../../VampireSurvivors/Art/Coins/CoinsSpriteSheet.png";
        auto resourceId = ResourceId(file);
        auto pSpriteTexture = gameManager.GetManager<ResourceManager>()->GetTexture(resourceId);
        if (pSpriteTexture)
        {
            pSpriteComp->SetSprite(pSpriteTexture, sf::Vector2f(1.f, 1.f));
            pSpriteComp->GetSprite().setTextureRect(sf::IntRect(0, 0, 6, 6));
            pSpriteComp->GetSprite().setOrigin(3.f, 5.f); // Center horizontally, slightly lower vertically
        }
        pSpriteComp->SetPosition(position);

        auto pAnimComp = pCoinDrop->GetComponent<SpriteAnimationComponent>().lock();
        if (!pAnimComp)
        {
            pAnimComp = std::make_shared<SpriteAnimationComponent>(pCoinDrop, gameManager);
            pCoinDrop->AddComponent(pAnimComp);
        }

        Animation coinSpinAnim;
        coinSpinAnim.frames = {
            sf::IntRect(0, 0, 6, 6),
            sf::IntRect(6, 0, 6, 6),
            sf::IntRect(12, 0, 6, 6)
        };
        coinSpinAnim.frameTime = 0.2f;

        pAnimComp->AddAnimation(EAnimationState::Move, coinSpinAnim);
        pAnimComp->PlayAnimation(EAnimationState::Move);
    }

    pCoinDrop->CreateBoxShapePhysicsBody(
        &gameManager.GetPhysicsWorld(),
        pCoinDrop->GetSize(),
        true,                   // isDynamic
        true                   // isSensor
    );

    auto pCollisionComp = std::make_shared<CollisionComponent>(
        pCoinDrop,
        gameManager,
        &gameManager.GetPhysicsWorld(),
        pCoinDrop->GetPhysicsBody(),
        pCoinDrop->GetSize(),
        true
    );

    pCoinDrop->AddComponent(pCollisionComp);

    auto * pPlayerManager = gameManager.GetManager<PlayerManager>();
    if (pPlayerManager)
    {
        auto & players = pPlayerManager->GetPlayers();
        if (!players.empty())
        {
            BD::Handle playerHandle = players[0];
            GameObject * pPlayer = gameManager.GetGameObject(playerHandle);
            if (pPlayer)
            {
                auto pRadiusPickupComponent = std::make_shared<RadiusPickupComponent>(pCoinDrop, gameManager, playerHandle, mRadius);
                pCoinDrop->AddComponent(pRadiusPickupComponent);
            }
        }
    }

    auto pLightComponent = pCoinDrop->GetComponent<LightComponent>().lock();
    if (!pLightComponent)
    {
        pLightComponent = std::make_shared<LightComponent>(pCoinDrop, gameManager, 5.f, sf::Color(255, 215, 0, 180));
        pCoinDrop->AddComponent(pLightComponent);
    }
}

//------------------------------------------------------------------------------------------------------------------------

void DropManager::MultRadius(float mult)
{
    mRadius *= mult;
}

//------------------------------------------------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------------------------------------------------