#include "AstroidsPrivate.h"
#include "AudioManager.h"

AudioManager::AudioManager(GameManager * pGameManager)
    : BaseManager(pGameManager)
    , mSoundPool()
{
}

//------------------------------------------------------------------------------------------------------------------------

AudioManager::~AudioManager()
{
}

//------------------------------------------------------------------------------------------------------------------------

void AudioManager::Update(float deltaTime)
{
    /*mSounds.erase(std::remove_if(mSounds.begin(), mSounds.end(),
        [](const sf::Sound & s) { return s.getStatus() == sf::Sound::Stopped; }),
        mSounds.end());*/
}

//------------------------------------------------------------------------------------------------------------------------

void AudioManager::OnGameEnd()
{
}

//------------------------------------------------------------------------------------------------------------------------

void AudioManager::PlayPooledSound(std::shared_ptr<sf::SoundBuffer> pBuffer, float volume, float pitch)
{
    for (const sf::Sound & sound : mSoundPool)
    {
        if (sound.getBuffer() == pBuffer.get() && sound.getStatus() == sf::Sound::Playing)
        {
            //return;
        }
    }

    for (sf::Sound & sound : mSoundPool)
    {
        if (sound.getStatus() == sf::Sound::Stopped)
        {
            sound.setBuffer(*pBuffer);
            sound.setVolume(volume);
            sound.setPitch(pitch);
            sound.play();
            return;
        }
    }

    // Optional: If no free sound, expand pool (or skip to cap usage)
    if (mSoundPool.size() < kSoundPoolSize)
    {
        sf::Sound newSound;
        newSound.setBuffer(*pBuffer);
        newSound.setVolume(volume);
        newSound.setPitch(pitch);
        newSound.play();
        mSoundPool.emplace_back(std::move(newSound));
    }
}

//------------------------------------------------------------------------------------------------------------------------

void AudioManager::TrackAndPlaySound(sf::Sound && sound)
{
    /*mSounds.emplace_back(std::move(sound));
    mSounds.back().play();*/
}

//------------------------------------------------------------------------------------------------------------------------

bool AudioManager::PlayBackgroundMusic(const std::string & filePath, float volume, bool loop)
{
    if (!mBackgroundMusic.openFromFile(filePath))
    {
        std::cerr << "Failed to load background music: " << filePath << "\n";
        return false;
    }

    mBackgroundMusic.setVolume(volume);
    mBackgroundMusic.setLoop(loop);
    mBackgroundMusic.play();
    return true;
}

//------------------------------------------------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------------------------------------------------