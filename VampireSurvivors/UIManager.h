#pragma once

#include "BaseManager.h"


struct DamageNumber
{
    sf::Text text;
    sf::Vector2f velocity;
    float lifespan;
    float elapsed;
    float startAlpha;

    DamageNumber(const sf::Font & font, const sf::Vector2f & pos, float amount)
        : velocity(0.f, -30.f) // upward
        , lifespan(1.0f)       // total seconds before disappearing
        , elapsed(0.f)
        , startAlpha(255.f)
    {
        text.setFont(font);
        text.setString(std::to_string(int(amount)));
        text.setCharacterSize(24);
        text.setFillColor(sf::Color::Red);
        text.setPosition(pos);
    }

    void Update(float deltaTime)
    {
        elapsed += deltaTime;
        text.move(velocity * deltaTime);

        float alpha = startAlpha * (1.0f - (elapsed / lifespan));
        sf::Color color = text.getFillColor();
        color.a = sf::Uint8(alpha);
        text.setFillColor(color);
    }

    bool IsExpired() const
    {
        return elapsed >= lifespan;
    }
};

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

	void AddDamageNumber(const sf::Vector2f & pos, float amount);

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

	std::vector<DamageNumber> mDamageNumbers;
};

//------------------------------------------------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------------------------------------------------