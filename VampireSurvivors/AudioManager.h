#pragma once
#include "BaseManager.h"
class AudioManager : public BaseManager
{
public:
    AudioManager(GameManager * pGameManager);
    ~AudioManager();

    virtual void Update(float deltaTime);
    virtual void OnGameEnd();

    void PlayPooledSound(std::shared_ptr<sf::SoundBuffer> pBuffer, float volume = 20.f, float pitch = 1.f);
    void TrackAndPlaySound(sf::Sound && sound);

    bool PlayBackgroundMusic(const std::string & filePath, float volume, bool loop);
private:
    static constexpr size_t kSoundPoolSize = 64;
    std::vector<sf::Sound> mSoundPool;
    sf::Music mBackgroundMusic;
};

