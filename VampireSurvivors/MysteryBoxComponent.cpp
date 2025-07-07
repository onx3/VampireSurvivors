#include "AstroidsPrivate.h"
#include "MysteryBoxComponent.h"
#include "PlayerManager.h"

MysteryBoxComponent::MysteryBoxComponent(GameObject * pOwner, GameManager & gameManger)
    : GameComponent(pOwner, gameManger)
    , mCoolDown(4.f)
    , mCurrentCooldown(0.f)
    , mInteractionRange(50.f)
    , mIsPlayerInRange(false)
    , mIsActive(true)
    , mPrice(500)
    , mBoxState(EBoxState::Idle)
    , mFlickerTimer(0.f)
    , mFlickerDuration(2.f)
    , mFlickerInterval(.1f)
    , mTimeSinceLastFlicker(0.f)
    , mPickupWaitTimer(5.f)
    , mCurrentFlickerWeapon(EWeaponType::Pistol)
    , mFinalSelectedWeapon(EWeaponType::Pistol)
    , mName("MysteryBoxComponent")
{
    mInteractionText.setFont(mFont);
    mInteractionText.setCharacterSize(15);
    mInteractionText.setFillColor(sf::Color::White);
    char buffer[64];
    std::snprintf(buffer, sizeof(buffer), "Press F to use mystery box");
    mInteractionText.setString(buffer);
}

//------------------------------------------------------------------------------------------------------------------------

MysteryBoxComponent::~MysteryBoxComponent()
{
}

//------------------------------------------------------------------------------------------------------------------------

void MysteryBoxComponent::Update(float deltaTime)
{
    GameManager & gameManager = GetGameManager();
    GameObject & gameObj = GetGameObject();

    if (!mIsActive)
    {
        mCurrentCooldown -= deltaTime;
        if (mCurrentCooldown <= 0.f)
        {
            mIsActive = true;
            mCurrentCooldown = 0.f;
        }
    }

    if (!mIsActive)
    {
        return;
    }

    auto * pPlayerManager = gameManager.GetManager<PlayerManager>();
    if (!pPlayerManager)
    {
        return;
    }

    BD::Handle playerHandle = pPlayerManager->GetActivePlayerHandle();
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
    }

    if (mBoxState == EBoxState::Flickering)
    {
        mFlickerTimer -= deltaTime;
        mTimeSinceLastFlicker += deltaTime;

        auto * pWeaponManager = gameManager.GetManager<WeaponManager>();
        if (mTimeSinceLastFlicker >= mFlickerInterval && pWeaponManager)
        {
            mTimeSinceLastFlicker = 0.f;
            const auto & weapons = pWeaponManager->GetAvailableWeapons();
            if (!weapons.empty())
            {
                mCurrentFlickerWeapon = weapons[rand() % weapons.size()];
                const WeaponInfo * pInfo = pWeaponManager->GetWeaponInfo(mCurrentFlickerWeapon);
                if (pInfo)
                {
                    ResourceId resourceId(pInfo->spritePath);
                    auto pTexture = gameManager.GetManager<ResourceManager>()->GetTexture(resourceId);
                    if (pTexture)
                    {
                        SetDisplayWeaponTexture(pTexture);
                    }
                }
            }
        }

        if (mFlickerTimer <= 0.f)
        {
            mFinalSelectedWeapon = mCurrentFlickerWeapon;
            mBoxState = EBoxState::WaitingToPickup;
            mPickupWaitTimer = 5.f;
        }

        return;
    }

    if (mBoxState == EBoxState::WaitingToPickup)
    {
        mPickupWaitTimer -= deltaTime;

        if (mPickupWaitTimer <= 0.f)
        {
            mDisplayTexture.reset();
            mBoxState = EBoxState::Idle;
            StartCooldown();
            return;
        }

        if (mIsPlayerInRange && gameManager.mInputHandler.IsKeyJustPressed(sf::Keyboard::F))
        {
            auto * pWeaponManager = gameManager.GetManager<WeaponManager>();
            if (pWeaponManager)
            {
                pWeaponManager->GiveWeaponToPlayer(*pPlayer, mFinalSelectedWeapon);
            }
            mDisplayTexture.reset();
            mBoxState = EBoxState::Idle;
            StartCooldown();
            return;
        }

        return;
    }

    if (mBoxState == EBoxState::Idle && mIsPlayerInRange && gameManager.mInputHandler.IsKeyJustPressed(sf::Keyboard::F))
    {
        auto * pWeaponManager = gameManager.GetManager<WeaponManager>();
        if (pWeaponManager)
        {
            const auto & availableWeapons = pWeaponManager->GetAvailableWeapons();
            if (!availableWeapons.empty())
            {
                int index = rand() % availableWeapons.size();
                mCurrentFlickerWeapon = availableWeapons[index];
                mFinalSelectedWeapon = mCurrentFlickerWeapon;
            }
        }

        mBoxState = EBoxState::Flickering;
        mFlickerTimer = mFlickerDuration;
        mTimeSinceLastFlicker = 0.f;
    }
}

//------------------------------------------------------------------------------------------------------------------------

void MysteryBoxComponent::StartCooldown()
{
    mIsActive = false;
    mCurrentCooldown = mCoolDown;

    auto pSpriteComp = GetGameObject().GetComponent<SpriteComponent>().lock();
    if (pSpriteComp)
    {
        pSpriteComp->GetSprite().setColor(sf::Color(100, 100, 100, 255));
    }

    mDisplayTexture.reset();
}

//------------------------------------------------------------------------------------------------------------------------

void MysteryBoxComponent::SetDisplayWeaponTexture(std::shared_ptr<sf::Texture> texture)
{
    mDisplayTexture = texture;
    if (mDisplayTexture)
    {
        mDisplaySprite.setTexture(*mDisplayTexture);
        mDisplaySprite.setOrigin(mDisplayTexture->getSize().x / 2.f, mDisplayTexture->getSize().y / 2.f);
        mDisplaySprite.setScale(sf::Vector2f(.4f, .4f));
    }
}

//------------------------------------------------------------------------------------------------------------------------

void MysteryBoxComponent::draw(sf::RenderTarget & target, sf::RenderStates states)
{
    if (mIsPlayerInRange)
    {
        target.draw(mInteractionText, states);
    }

    if (mDisplayTexture && (mBoxState == EBoxState::Flickering || mBoxState == EBoxState::WaitingToPickup))
    {
        mDisplaySprite.setPosition(GetGameObject().GetPosition() + sf::Vector2f(0.f, -32.f));
        target.draw(mDisplaySprite, states);
    }
}

//------------------------------------------------------------------------------------------------------------------------

void MysteryBoxComponent::DebugImGuiComponentInfo()
{
}

//------------------------------------------------------------------------------------------------------------------------

const std::string & MysteryBoxComponent::GetClassName()
{
    return mName;
}

//------------------------------------------------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------------------------------------------------