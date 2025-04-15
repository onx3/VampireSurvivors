#pragma once

#include "BaseManager.h"

class GameManager;
class UIManager : public BaseManager
{
public:
	UIManager(GameManager * pGameManager);

	virtual void Update(float deltaTime) override;
	virtual void Render(sf::RenderWindow & window) override;

	void AddScore(int points);
	const sf::Text & GetScoreText();

	float GetPlayerHelath();

	std::vector<sf::Sprite> & GetSpriteLives();
	int GetScore() const;

	const sf::Text & GetRunTimeText();
	float GetRunTime() const;

private:
	int mScore;
	sf::Font mFont;
	sf::Text mScoreText;

	float mHealth;
	sf::Text mHealthText;

	float mRunTime;
	sf::Text mRunTimeText;

	sf::Texture mLifeTexture;
	sf::Sprite mLifeSprite;
	std::vector<sf::Sprite> mSpriteLives;
};

//------------------------------------------------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------------------------------------------------