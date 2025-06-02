#include "AstroidsPrivate.h"
#include "RoundManager.h"
#include "LevelManager.h"
#include "PlayerManager.h"
#include <random>

RoundManager::RoundManager(GameManager * pGameManager)
    : BaseManager(pGameManager)
    , mRound(1)
    , mEnemiesLeftToSpawn(0)
    , mEnemiesAlive(0)
    , mMaxAliveEnemies(25)
    , mSpawnTimer(0.f)
    , mTimeBetweenSpawns(0.f)
    , mInterRoundTimer(3.f)
    , mLastUsedSpawnPos(0.f, 0.f)
{
    mEnemiesLeftToSpawn = GetEnemiesToSpawnForRound(mRound);
}

//------------------------------------------------------------------------------------------------------------------------

RoundManager::~RoundManager()
{
}

//------------------------------------------------------------------------------------------------------------------------

void RoundManager::Update(float deltaTime)
{
    if (mInterRoundTimer > 0.f)
    {
        mInterRoundTimer -= deltaTime;
        return;
    }

    if (mEnemiesLeftToSpawn > 0)
    {
        mSpawnTimer -= deltaTime;
        if (mSpawnTimer <= 0.f && mEnemiesAlive < mMaxAliveEnemies)
        {
            SpawnEnemy();
            --mEnemiesLeftToSpawn;
            ++mEnemiesAlive;
            mSpawnTimer = mTimeBetweenSpawns;
        }
    }
    else if (mEnemiesAlive == 0)
    {
        StartNextRound();
    }
}

//------------------------------------------------------------------------------------------------------------------------

void RoundManager::OnEnemyKilled()
{
    mEnemiesAlive = std::max(0, mEnemiesAlive - 1);
}

//------------------------------------------------------------------------------------------------------------------------

int RoundManager::GetCurrentRound() const
{
    return mRound;
}

//------------------------------------------------------------------------------------------------------------------------

void RoundManager::StartNextRound()
{
    ++mRound;
    mEnemiesLeftToSpawn = GetEnemiesToSpawnForRound(mRound);
    mEnemiesAlive = 0;
    mSpawnTimer = 0.f;
    mInterRoundTimer = 5.f;

    auto * pUI = GetGameManager().GetManager<UIManager>();
    if (pUI)
    {
        pUI->ShowRoundIntro(mRound);
    }
}

//------------------------------------------------------------------------------------------------------------------------

void RoundManager::SpawnEnemy() 
{
    const sf::Vector2f & spawnPos = ChooseValidSpawn();
    EEnemy enemyType = ChooseEnemyTypeForRound(mRound);
    auto * pEnemyManager = GetGameManager().GetManager<EnemyAIManager>();
    if (pEnemyManager)
    {
        pEnemyManager->RespawnEnemy(enemyType, spawnPos);
    }
}

//------------------------------------------------------------------------------------------------------------------------

int RoundManager::GetEnemiesToSpawnForRound(int round)
{
    return 5 + (round * 3); // Random values
}

//------------------------------------------------------------------------------------------------------------------------

const sf::Vector2f & RoundManager::ChooseValidSpawn()
{
    auto * pPlayerManager = GetGameManager().GetManager<PlayerManager>();
    assert(pPlayerManager && "PlayerManager is null");

    const RoomData * pRoomData = pPlayerManager->GetCurrentRoom();

    if (!pRoomData || pRoomData->enemySpawnPositions.empty())
    {
        return mLastUsedSpawnPos;
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, int(pRoomData->enemySpawnPositions.size()) - 1);
    int randomIndex = dist(gen);

    mLastUsedSpawnPos = pRoomData->enemySpawnPositions[randomIndex];
    return mLastUsedSpawnPos;
}

//------------------------------------------------------------------------------------------------------------------------

EEnemy RoundManager::ChooseEnemyTypeForRound(int round)
{
    auto * pEnemyManager = GetGameManager().GetManager<EnemyAIManager>();
    if (!pEnemyManager)
    {
        return EEnemy::Chort;
    }
    // Every 5 rounds is a boss round
    if (round % 5)
    {
        
    }
    //else
    {        
        return pEnemyManager->GetEnemyType();
    }
}

//------------------------------------------------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------------------------------------------------