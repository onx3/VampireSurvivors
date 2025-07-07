#include "AstroidsPrivate.h"
#include "WallBuyWeaponComponent.h"
#include "PlayerManager.h"
#include "WeaponInventoryComponent.h"

WallBuyWeaponComponent::WallBuyWeaponComponent(GameObject * pOwner, GameManager & gameManager, const std::string & weaponType, int price)
    : GameComponent(pOwner, gameManager)
    , mPrice(price)
    , mInteractionRange(50.f)
    , mIsPlayerInRange(false)
    , mName("WallBuyWeaponComponent")
{
    mWeaponType = WeaponComponent::ConvertStringToWeaponType(weaponType);

    if (!mFont.loadFromFile("../../VampireSurvivors/Art/font.ttf"))
    {
        std::cerr << "Failed to load font\n";
    }

    mInteractionText.setFont(mFont);
    mInteractionText.setCharacterSize(15);
    mInteractionText.setFillColor(sf::Color::White);

    auto pSpriteComp = GetGameObject().GetComponent<SpriteComponent>().lock();
    if (pSpriteComp)
    {
        auto * pWeaponManager = gameManager.GetManager<WeaponManager>();
        if (pWeaponManager)
        {
            const WeaponInfo * pInfo = pWeaponManager->GetWeaponInfo(mWeaponType);
            if (pInfo)
            {
                ResourceId resourceId(pInfo->spritePath);
                auto pTexture = gameManager.GetManager<ResourceManager>()->GetTexture(resourceId);
                if (pTexture)
                {
                    auto pSpriteComp = GetGameObject().GetComponent<SpriteComponent>().lock();
                    if (pSpriteComp)
                    {
                        pSpriteComp->SetSprite(pTexture, sf::Vector2f(.5f, .5f));
                        pSpriteComp->GetSprite().setTextureRect(sf::IntRect(0, 0, 96, 48));
                        pSpriteComp->SetPosition(GetGameObject().GetPosition());
                        pSpriteComp->GetSprite().setColor(sf::Color(0, 255, 0, 255));
                    }
                }
                char buffer[64];
                std::snprintf(buffer, sizeof(buffer), "Press F to buy %s ($%d)", pInfo->name.c_str(), pInfo->price);
                mInteractionText.setString(buffer);
            }
        }
    }
}

//------------------------------------------------------------------------------------------------------------------------

WallBuyWeaponComponent::~WallBuyWeaponComponent()
{
}

//------------------------------------------------------------------------------------------------------------------------

void WallBuyWeaponComponent::Update(float deltaTime)
{
    GameManager & gameManager = GetGameManager();
    GameObject & gameObj = GetGameObject();

    auto * pPlayerManager = gameManager.GetManager<PlayerManager>();
    if (!pPlayerManager)
    {
        return;
    }

    auto & players = pPlayerManager->GetPlayers();
    if (players.empty())
    {
        return;
    }

    BD::Handle playerHandle = players[0];
    GameObject * pPlayer = gameManager.GetGameObject(playerHandle);
    if (!pPlayer)
    {
        return;
    }

    float distanceSqr = BD::GetMagnitudeSquared(gameObj.GetPosition(), pPlayer->GetPosition());
    mIsPlayerInRange = (distanceSqr <= mInteractionRange * mInteractionRange);

    if (mIsPlayerInRange)
    {
        mInteractionText.setPosition(gameObj.GetPosition().x, gameObj.GetPosition().y - 40.f);

        if (gameManager.mInputHandler.IsKeyJustPressed(sf::Keyboard::F))
        {
            BuyWeapon(*pPlayer);
        }
    }
}

//------------------------------------------------------------------------------------------------------------------------

void WallBuyWeaponComponent::draw(sf::RenderTarget & target, sf::RenderStates states)
{
    if (mIsPlayerInRange)
    {
        target.draw(mInteractionText);
    }
}

//------------------------------------------------------------------------------------------------------------------------

void WallBuyWeaponComponent::DebugImGuiComponentInfo()
{
    
}

//------------------------------------------------------------------------------------------------------------------------

const std::string & WallBuyWeaponComponent::GetClassName()
{
    return mName;
}

//------------------------------------------------------------------------------------------------------------------------

void WallBuyWeaponComponent::BuyWeapon(GameObject & playerObj)
{
    GameManager & gameManager = GetGameManager();

    auto * pWeaponManager = gameManager.GetManager<WeaponManager>();
    if (pWeaponManager)
    {
        pWeaponManager->GiveWeaponToPlayer(playerObj, mWeaponType);
    }
}

//------------------------------------------------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------------------------------------------------