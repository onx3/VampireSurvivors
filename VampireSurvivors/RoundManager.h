#pragma once
#include "BaseManager.h"
class RoundManager : public BaseManager
{
public:
    RoundManager(GameManager * pGameManager);
    ~RoundManager();

    virtual void Update(float deltaTime) override;
    void OnEnemyKilled();

    int GetCurrentRound() const;

private:
    void StartNextRound();
    void SpawnEnemy();
    int GetEnemiesToSpawnForRound(int round);
    const sf::Vector2f & ChooseValidSpawn();
    EEnemy ChooseEnemyTypeForRound(int round);

    int mRound;
    int mEnemiesLeftToSpawn;
    int mEnemiesAlive;
    int mMaxAliveEnemies;

    float mSpawnTimer;
    float mTimeBetweenSpawns;
    float mInterRoundTimer;
    sf::Vector2f mLastUsedSpawnPos;
};

