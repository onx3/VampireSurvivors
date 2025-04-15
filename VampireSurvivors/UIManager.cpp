#include "AstroidsPrivate.h"
#include "UIManager.h"
#include "HealthComponent.h"
#include <cassert>
#include "PlayerManager.h"

UIManager::UIManager(GameManager * pGameManager)
	: BaseManager(pGameManager)
    , mScore(0)
    , mRunTime(0.f)
	, mSpriteLives()
{
	if (!mFont.loadFromFile("Art/font.ttf")) // Replace with your font path
	{
		assert(false && "Failed to load font");
	}
	mScoreText.setFont(mFont);
	mScoreText.setCharacterSize(24);
	mScoreText.setFillColor(sf::Color::Cyan);
	mScoreText.setOutlineColor(sf::Color::Black);
	mScoreText.setPosition(10.f, 10.f); // Top-left corner
	mScoreText.setString("Score: 0");

    mRunTimeText.setFont(mFont);
    mRunTimeText.setCharacterSize(24);
    mRunTimeText.setFillColor(sf::Color::Cyan);
    mRunTimeText.setOutlineColor(sf::Color::Black);
    mRunTimeText.setString("Time: 0.0s");

	assert(mLifeTexture.loadFromFile("Art/UI/ui_heart_full.png"));
	mLifeSprite.setTexture(mLifeTexture);
    mLifeSprite.setScale(sf::Vector2f(1.2f, 1.2f));
}

//------------------------------------------------------------------------------------------------------------------------

void UIManager::Update(float deltaTime)
{
    mRunTime += deltaTime;
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "Time: %.1fs", mRunTime);
    mRunTimeText.setString(buffer);
}

//------------------------------------------------------------------------------------------------------------------------

void UIManager::Render(sf::RenderWindow & window)
{
    const sf::View & view = window.getView();
    sf::Vector2f viewSize = view.getSize();
    sf::Vector2f viewTopLeft = view.getCenter() - (viewSize / 2.f);

    mScoreText.setPosition(viewTopLeft.x + 10.f, viewTopLeft.y + 10.f);

    sf::FloatRect bounds = mRunTimeText.getLocalBounds();
    mRunTimeText.setPosition(viewTopLeft.x + (viewSize.x - bounds.width) / 2.f, viewTopLeft.y + 10.f);

    window.draw(mScoreText);
    window.draw(mRunTimeText);

    auto & lives = GetSpriteLives();
    for (auto & life : lives)
    {
        window.draw(life);
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

std::vector<sf::Sprite> & UIManager::GetSpriteLives()
{
    mSpriteLives.clear(); // Clear existing sprites
    sf::Vector2f viewTopLeft(0.f, 0.f);

    auto & window = GetGameManager().GetWindow();
    viewTopLeft = window.getView().getCenter() - window.getView().getSize() / 2.f;

    sf::Vector2f lifeStartPos(viewTopLeft.x + 10.f, viewTopLeft.y + 50.f);

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
// EOF
//------------------------------------------------------------------------------------------------------------------------