#pragma once

class GameManager;

class BaseManager
{
public:
	explicit BaseManager(GameManager * pGameManager);
	~BaseManager();

	virtual void Update(float deltaTime);
	virtual void Render(sf::RenderWindow & window);
	virtual void OnGameEnd();

	GameManager & GetGameManager() const;
private:
	GameManager * mpGameManager;
};

//------------------------------------------------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------------------------------------------------