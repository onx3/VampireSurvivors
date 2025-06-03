#include "AstroidsPrivate.h"
#include "UIManager.h"
#include "HealthComponent.h"
#include <cassert>
#include "PlayerManager.h"
#include "RoundManager.h"
#include "ReloadComponent.h"

UIManager::UIManager(GameManager * pGameManager)
	: BaseManager(pGameManager)
    , mScore(0)
    , mHealth(0.f)
    , mRunTime(0.f)
	, mSpriteLives()
    , mReloadFlashTimer(0.f)
    , mReloadFlashSpeed(8.f)
{
	if (!mFont.loadFromFile("../../VampireSurvivors/Art/youmurdererbb_reg.ttf"))
	{
		assert(false && "Failed to load font");
	}

    // Score Info
	mScoreText.setFont(mFont);
	mScoreText.setCharacterSize(32);
	mScoreText.setFillColor(sf::Color::Cyan);
	mScoreText.setOutlineColor(sf::Color::Black);
	mScoreText.setPosition(10.f, 10.f); // Top-left corner
	mScoreText.setString("Score: 0");

    mRunTimeText.setFont(mFont);
    mRunTimeText.setCharacterSize(32);
    mRunTimeText.setFillColor(sf::Color::Cyan);
    mRunTimeText.setOutlineColor(sf::Color::Black);
    mRunTimeText.setString("Time: 0.0s");

    // Health Info
    mHealthText.setFont(mFont);
    mHealthText.setCharacterSize(32);
    mHealthText.setFillColor(sf::Color::Red);
    mHealthText.setOutlineColor(sf::Color::Black);
    mHealthText.setString("Health: ");
	assert(mLifeTexture.loadFromFile("../../VampireSurvivors/Art/UI/ui_heart_full.png"));
	mLifeSprite.setTexture(mLifeTexture);
    mLifeSprite.setScale(sf::Vector2f(1.2f, 1.2f));

    // Round Info
    mRoundText.setFont(mFont);
    mRoundText.setCharacterSize(32);
    mRoundText.setFillColor(sf::Color::White);
    mRoundText.setOutlineColor(sf::Color(128, 0, 0));
    mRoundText.setOutlineColor(sf::Color::Black);
    mRoundText.setString("Round: 1");

    mRoundIntroText.setFont(mFont);
    mRoundIntroText.setCharacterSize(72);
    mRoundIntroText.setFillColor(sf::Color::White);
    mRoundIntroText.setOutlineColor(sf::Color::Black);
    mRoundIntroText.setString("");

    // Ammo Info
    mAmmoText.setFont(mFont);
    mAmmoText.setCharacterSize(32);
    mAmmoText.setFillColor(sf::Color::White);
    mAmmoText.setOutlineColor(sf::Color::Black);
    mAmmoText.setString("Ammo: --/--");
}

//------------------------------------------------------------------------------------------------------------------------

void UIManager::Update(float deltaTime)
{
    mRunTime += deltaTime;
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "Time: %.1fs", mRunTime);
    mRunTimeText.setString(buffer);

    mHealth = GetPlayerHelath();
    mHealthText.setString("Health: " + std::to_string(int(mHealth)));

    // Damage numbers
    {
        for (auto & dn : mDamageNumbers)
        {
            dn.Update(deltaTime);
        }

        mDamageNumbers.erase(
            std::remove_if(mDamageNumbers.begin(), mDamageNumbers.end(),
                [](const DamageNumber & dn) { return dn.IsExpired(); }),
            mDamageNumbers.end());
    }

    // Round Text
    {
        auto * pRoundManager = GetGameManager().GetManager<RoundManager>();
        if (pRoundManager)
        {
            int round = pRoundManager->GetCurrentRound();
            mRoundText.setString("Round: " + std::to_string(round));
        }
        if (mRoundIntroTimer > 0.f)
        {
            mRoundIntroTimer -= deltaTime;
            float time = 2.0f - mRoundIntroTimer;

            float alpha = 255.f;

            if (time < 0.5f)
            {
                // Fade in
                alpha = 255.f * (time / 0.5f);
            }
            else if (time > 1.5f)
            {
                // Fade out
                alpha = 255.f * ((2.0f - time) / 0.5f);
            }

            sf::Color color = mRoundIntroText.getFillColor();
            color.a = static_cast<sf::Uint8>(std::clamp(alpha, 0.f, 255.f));
            mRoundIntroText.setFillColor(color);
        }
        else
        {
            mRoundIntroText.setString("");
        }
    }

    // Ammo Text
    {
        auto * pPlayerManager = GetGameManager().GetManager<PlayerManager>();
        if (pPlayerManager && !pPlayerManager->GetPlayers().empty())
        {
            GameObject * pPlayer = GetGameManager().GetGameObject(pPlayerManager->GetPlayers()[0]);
            if (pPlayer)
            {
                auto pReload = pPlayer->GetComponent<ReloadComponent>().lock();
                if (pReload)
                {
                    std::string ammoStr;
                    if (pReload->GetReserveAmmo() == -1)
                    {
                        ammoStr = "Ammo: " + std::to_string(pReload->GetClipAmmo()) + "/INF";
                    }
                    else
                    {
                        ammoStr = "Ammo: " + std::to_string(pReload->GetClipAmmo()) + "/" + std::to_string(pReload->GetReserveAmmo());
                    }
                    mAmmoText.setString(ammoStr);

                    if (pReload->IsReloading())
                    {
                        mReloadFlashTimer += deltaTime;
                        float alpha = 128.f + 127.f * std::sin(mReloadFlashTimer * mReloadFlashSpeed);

                        sf::Color flashColor = sf::Color::Red;
                        flashColor.a = static_cast<sf::Uint8>(std::clamp(alpha, 0.f, 255.f));
                        mAmmoText.setFillColor(flashColor);
                    }
                    else
                    {
                        mReloadFlashTimer = 0.f;
                        mAmmoText.setFillColor(sf::Color::White);
                    }
                }
            }
        }
    }
}

//------------------------------------------------------------------------------------------------------------------------

void UIManager::Render(sf::RenderWindow & window)
{
    const sf::View & view = window.getView();
    sf::Vector2f viewSize = view.getSize();
    sf::Vector2f viewTopLeft = view.getCenter() - (viewSize / 2.f);

    mScoreText.setPosition(viewTopLeft.x + 10.f, viewTopLeft.y + 10.f);
    mHealthText.setPosition(viewTopLeft.x + 10.f, viewTopLeft.y + 30.f);

    sf::FloatRect bounds = mRunTimeText.getLocalBounds();
    mRunTimeText.setPosition(viewTopLeft.x + (viewSize.x - bounds.width) / 2.f, viewTopLeft.y + 10.f);

    window.draw(mScoreText);
    window.draw(mHealthText);
    window.draw(mRunTimeText);

    auto & lives = GetSpriteLives();
    for (auto & life : lives)
    {
        window.draw(life);
    }

    // Damage numbers
    {
        for (const auto & dn : mDamageNumbers)
        {
            window.draw(dn.text);
        }
    }

    // Round Info
    {
        sf::FloatRect roundBounds = mRoundText.getLocalBounds();
        mRoundText.setPosition(viewTopLeft.x + (viewSize.x - roundBounds.width) / 2.f, viewTopLeft.y + 40.f);
        window.draw(mRoundText);
        if (mRoundIntroTimer > 0.f)
        {
            sf::FloatRect bounds = mRoundIntroText.getLocalBounds();
            mRoundIntroText.setPosition(viewTopLeft.x + (viewSize.x - bounds.width) / 2.f,
                viewTopLeft.y + viewSize.y / 2.f - bounds.height / 2.f);
            window.draw(mRoundIntroText);
        }
    }

    // Ammo Info
    {
        sf::FloatRect ammoBounds = mAmmoText.getLocalBounds();
        mAmmoText.setPosition(
            viewTopLeft.x + viewSize.x - ammoBounds.width - 20.f,
            viewTopLeft.y + viewSize.y - ammoBounds.height - 20.f
        );
        window.draw(mAmmoText);
    }
}

//------------------------------------------------------------------------------------------------------------------------

void UIManager::AddScore(int points)
{
	mScore += points;
	mScoreText.setString("Score: " + std::to_string(mScore));
}

//------------------------------------------------------------------------------------------------------------------------

const sf::Text & UIManager::GetScoreText()
{
	return mScoreText;
}

//------------------------------------------------------------------------------------------------------------------------

float UIManager::GetPlayerHelath()
{
    float health = 0.f;
    auto * pPlayerManager = GetGameManager().GetManager<PlayerManager>();
    if (!pPlayerManager)
    {
        return health;
    }

    auto & players = pPlayerManager->GetPlayers();
    if (players.empty())
    {
        return health;
    }

    BD::Handle playerHandle = players[0];
    GameObject * pPlayerObject = GetGameManager().GetGameObject(playerHandle);
    if (!pPlayerObject || pPlayerObject->IsDestroyed())
    {
        return health;
    }

    auto pHealthComp = pPlayerObject->GetComponent<HealthComponent>().lock();
    if (pHealthComp)
    {
        health = pHealthComp->GetHealth();
    }
    return health;
}

//------------------------------------------------------------------------------------------------------------------------

std::vector<sf::Sprite> & UIManager::GetSpriteLives()
{
    mSpriteLives.clear(); // Clear existing sprites
    sf::Vector2f viewTopLeft(0.f, 0.f);

    auto & window = GetGameManager().GetWindow();
    viewTopLeft = window.getView().getCenter() - window.getView().getSize() / 2.f;

    sf::Vector2f lifeStartPos(viewTopLeft.x + 10.f, viewTopLeft.y + 70.f);

    // Find the player GameObject and get its HealthComponent
    int lives = 0;
    auto * pPlayerManager = GetGameManager().GetManager<PlayerManager>();
    if (!pPlayerManager)
    {
        return mSpriteLives;
    }

    auto & players = pPlayerManager->GetPlayers();
    if (players.empty())
    {
        return mSpriteLives;
    }

    BD::Handle playerHandle = players[0];
    GameObject * pPlayerObject = GetGameManager().GetGameObject(playerHandle);
    if (!pPlayerObject || pPlayerObject->IsDestroyed())
    {
        return mSpriteLives;
    }

    auto pHealthComponent = pPlayerObject->GetComponent<HealthComponent>().lock();
    if (pHealthComponent)
    {
        lives = pHealthComponent->GetLives();
    }

    for (int ii = 0; ii < lives; ++ii)
    {
        sf::Sprite lifeSprite = mLifeSprite;
        lifeSprite.setPosition(lifeStartPos.x + ii * 20, lifeStartPos.y);
        mSpriteLives.push_back(lifeSprite);
    }

    return mSpriteLives;
}


//------------------------------------------------------------------------------------------------------------------------

int UIManager::GetScore() const
{
	return mScore;
}

//------------------------------------------------------------------------------------------------------------------------

void UIManager::RemoveScore(int removeScore)
{
    mScore -= removeScore;
}

//------------------------------------------------------------------------------------------------------------------------

const sf::Text & UIManager::GetRunTimeText()
{
    return mRunTimeText;
}

//------------------------------------------------------------------------------------------------------------------------

float UIManager::GetRunTime() const
{
    return mRunTime;
}

//------------------------------------------------------------------------------------------------------------------------

void UIManager::AddDamageNumber(const sf::Vector2f & pos, float amount, sf::Color & color)
{
    if (amount != 0)
    {
        mDamageNumbers.emplace_back(mFont, pos, amount, color);
    }
}

//------------------------------------------------------------------------------------------------------------------------

void UIManager::ShowRoundIntro(int roundNumber)
{
    mRoundIntroText.setString("Round " + std::to_string(roundNumber));
    mRoundIntroText.setFillColor(sf::Color::White);
    mRoundIntroTimer = 2.0f;
}

//------------------------------------------------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------------------------------------------------