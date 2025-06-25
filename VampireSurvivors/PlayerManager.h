#pragma once
#include "BaseManager.h"
#include "LevelManager.h"

class PlayerManager : public BaseManager
{
public:
    PlayerManager(GameManager * pGameManager);
    ~PlayerManager();

    void InitPlayer();

    virtual void Update(float deltaTime) override;
    virtual void OnGameEnd() override;

    void OnPlayerLostLife(GameObject * pPlayer);

    void OnPlayerDeath(GameObject * pPlayer);

    const std::vector<BD::Handle> & GetPlayers() const;
    BD::Handle GetActivePlayerHandle() const;

    GameObject * FindClosestEnemy();

    const RoomData * GetCurrentRoom() const;
private:
    void CreateAnimationComponent(GameObject & player);
    void AddWeaponGameObject(GameObject & player);

    std::vector<BD::Handle> mPlayerHandles;

    sf::Vector2f mSpawnPos;
    const RoomData * mpCurrentRoom;
    // Audio
    sf::SoundBuffer mLoseLifeSoundBuffer;
    sf::SoundBuffer mDeathSoundBuffer;
    sf::Sound mLoseLifeSound;
    sf::Sound mDeathSound;
    bool mSoundPlayed;
};
