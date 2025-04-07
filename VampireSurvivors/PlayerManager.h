#pragma once
#include "BaseManager.h"

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

private:
    std::vector<BD::Handle> mPlayerHandles;

    // Audio
    sf::SoundBuffer mLoseLifeSoundBuffer;
    sf::SoundBuffer mDeathSoundBuffer;
    sf::Sound mLoseLifeSound;
    sf::Sound mDeathSound;
    bool mSoundPlayed;
};
