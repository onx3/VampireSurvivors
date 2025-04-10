#pragma once
#include "BaseManager.h"
#include <vector>
#include "GameObject.h"
#include "DropManager.h"
#include "SpriteComponent.h"

namespace
{
	const int mkMaxEnemies = 35;
}

enum class EEnemy
{
	LizardF,
	Ogre
};

class EnemyAIManager : public BaseManager
{
public:
	EnemyAIManager(GameManager * pGameManager);
	~EnemyAIManager();

	virtual void Update(float deltaTime) override;
	virtual void OnGameEnd() override;

	void RemoveEnemy(GameObject * pEnemy);
	void RespawnEnemy(EEnemy type, sf::Vector2f pos);

	void AddEnemies(int count, EEnemy type, sf::Vector2f pos);
	void DestroyAllEnemies();

	const std::vector<BD::Handle> & GetEnemies() const;

	void OnDeath(GameObject * pEnemy);

private:
	std::string GetEnemyFile(EEnemy type);
	void CleanUpDeadEnemies();

	EDropType DetermineDropType() const;

	void SetUpSprite(SpriteComponent & spriteComp, EEnemy type);

	std::vector<BD::Handle> mEnemyHandles;
};

//------------------------------------------------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------------------------------------------------