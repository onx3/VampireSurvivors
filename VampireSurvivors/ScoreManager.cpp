#include "AstroidsPrivate.h"
#include "ScoreManager.h"
#include "HealthComponent.h"
#include <cassert>
#include "PlayerManager.h"

ScoreManager::ScoreManager(GameManager * pGameManager)
	: BaseManager(pGameManager)
    , mScore(0)
	, mSpriteLives()
{
	if (!mFont.loadFromFile("Art/font.ttf")) // Replace with your font path
	{
		assert(false && "Failed to load font");
	}
	mScoreText.setFont(mFont);
	mScoreText.setCharacterSize(24);
	mScoreText.setFillColor(sf::Color::Green);
	mScoreText.setOutlineColor(sf::Color::Black);
	mScoreText.setPosition(10.f, 10.f); // Top-left corner
	mScoreText.setString("Score: 0");

	assert(mLifeTexture.loadFromFile("Art/life.png"));
	mLifeSprite.setTexture(mLifeTexture);
}

//------------------------------------------------------------------------------------------------------------------------

void ScoreManager::Render(sf::RenderWindow & window)
{
	window.draw(GetScoreText());
	for (auto & life : mSpriteLives)
	{
		window.draw(life);
	}
}

//------------------------------------------------------------------------------------------------------------------------

void ScoreManager::AddScore(int points)
{
	mScore += points;
	mScoreText.setString("Score: " + std::to_string(mScore));
}

//------------------------------------------------------------------------------------------------------------------------

const sf::Text & ScoreManager::GetScoreText()
{
	return mScoreText;
}

//------------------------------------------------------------------------------------------------------------------------

std::vector<sf::Sprite> & ScoreManager::GetSpriteLives()
{
    mSpriteLives.clear(); // Clear existing sprites

    sf::Vector2f lifeStartPos(10, 50); // Starting position for lives display

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
        lifeSprite.setPosition(lifeStartPos.x + ii * 40, lifeStartPos.y);
        mSpriteLives.push_back(lifeSprite);
    }

    return mSpriteLives;
}


//------------------------------------------------------------------------------------------------------------------------

int ScoreManager::GetScore()
{
	return mScore;
}


//------------------------------------------------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------------------------------------------------