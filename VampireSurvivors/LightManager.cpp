#include "AstroidsPrivate.h"
#include "LightManager.h"
#include "LightComponent.h"

LightManager::LightManager(GameManager * pGameManager)
    : BaseManager(pGameManager)
    , mLightTexture()
    , mSoftLightTexture()
{
    std::string file = "../../VampireSurvivors/Art/Lighting/Circular.png";
    ResourceId resourceId(file);

    mSoftLightTexture = pGameManager->GetManager<ResourceManager>()->GetTexture(resourceId);
    if (mSoftLightTexture)
    {
        mSoftLightTexture->setSmooth(true);
    }
}

//------------------------------------------------------------------------------------------------------------------------

LightManager::~LightManager()
{
}

//------------------------------------------------------------------------------------------------------------------------

void LightManager::Update(float deltaTime)
{
}

//------------------------------------------------------------------------------------------------------------------------

void LightManager::Render(sf::RenderWindow & window)
{
    auto & gameManager = GetGameManager();
    sf::View currentView = window.getView();
    sf::Vector2f viewSize = currentView.getSize();
    
    sf::Vector2u lightTextureSize(
        static_cast<unsigned>(viewSize.x),
        static_cast<unsigned>(viewSize.y)
    );
    
    if (mLightTexture.getSize() != lightTextureSize)
    {
        if (!mLightTexture.create(lightTextureSize.x, lightTextureSize.y))
            return;
    }
    
    mLightTexture.setView(currentView);
    mLightTexture.clear(sf::Color(50, 50, 50, 255)); // Dim ambient lighting
    
    std::vector<GameObject *> gameObjects;
    gameObjects.reserve(200);
    gameManager.GetAllGameObjects(gameObjects);
    
    for (GameObject * pObj : gameObjects)
    {
        auto pLight = pObj->GetComponent<LightComponent>().lock();
        if (!pLight || !mSoftLightTexture)
            continue;
    
        sf::Sprite lightSprite;
        lightSprite.setTexture(*mSoftLightTexture);
        lightSprite.setOrigin(mSoftLightTexture->getSize().x / 2.f, mSoftLightTexture->getSize().y / 2.f);
        lightSprite.setPosition(pLight->GetPosition());
    
        float radius = pLight->GetRadius();
        float baseRadius = mSoftLightTexture->getSize().x / 2.f;
        float scale = radius / baseRadius;
        lightSprite.setScale(scale, scale);
        lightSprite.setColor(pLight->GetColor());
    
        mLightTexture.draw(lightSprite, sf::BlendAdd);
    }
    
    mLightTexture.display();
    
    sf::Sprite lightSprite(mLightTexture.getTexture());
    lightSprite.setPosition(currentView.getCenter() - (viewSize / 2.f));
    window.draw(lightSprite, sf::BlendMultiply);
}
//------------------------------------------------------------------------------------------------------------------------

void LightManager::OnGameEnd()
{
}

//------------------------------------------------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------------------------------------------------