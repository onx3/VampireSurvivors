#pragma once
#include "BaseManager.h"

class CameraManager : public BaseManager
{
public:
	CameraManager(GameManager * pGameManager);

	virtual void Update(float deltaTime) override;
	virtual void Render(sf::RenderWindow & window) override;
	virtual void OnGameEnd() override;

	sf::View & GetView();
	sf::Vector2f GetCrosshairPosition() const;

	static sf::Vector2f Lerp(sf::Vector2f start, sf::Vector2f end, float t);

private:
	sf::View mView;
	sf::Sprite mCursorSprite;
	sf::Vector2f mPreviousViewCenter;
};

