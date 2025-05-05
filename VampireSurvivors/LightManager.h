#pragma once
#include "BaseManager.h"
class LightManager : public BaseManager
{
public:
    LightManager(GameManager * pGameManager);
    ~LightManager();

    virtual void Update(float deltaTime) override;
    virtual void Render(sf::RenderWindow & window) override;
    virtual void OnGameEnd() override;

private:
    sf::RenderTexture mLightTexture;
    std::shared_ptr<sf::Texture> mSoftLightTexture;
};

