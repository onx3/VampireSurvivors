#include "AstroidsPrivate.h"
#include "LevelManager.h"
#include "ResourceManager.h"
#include <fstream>
#include <iostream>
#include "LightComponent.h"
#include "SpriteAnimationComponent.h"
#include "DoorComponent.h"
#include "CollisionComponent.h"

LevelManager::LevelManager(GameManager * pGameManager)
    : BaseManager(pGameManager)
    , mWidth(0)
    , mHeight(0)
    , mTileWidth(0)
    , mTileHeight(0)
    , mLevelData()
{
}

//------------------------------------------------------------------------------------------------------------------------

LevelManager::~LevelManager()
{
    ClearLevel();
}

//------------------------------------------------------------------------------------------------------------------------

bool LevelManager::LoadLevel(const std::string & filePath)
{
    std::ifstream file(filePath);
    if (!file.is_open())
    {
        std::cerr << "Failed to open file: " << filePath << std::endl;
        return false;
    }

    json levelData;
    file >> levelData;
    ParseTileData(levelData);
    return true;
}

//------------------------------------------------------------------------------------------------------------------------

void LevelManager::Render(sf::RenderWindow & window)
{
    for (const auto & layer : mTileLayers)
    {
        if (layer.texture)
        {
            window.draw(layer.vertices, layer.texture.get());
        }
    }
}

//------------------------------------------------------------------------------------------------------------------------

bool LevelManager::IsTileWalkableAI(int x, int y) const
{
    if (y < 0 || y >= mHeight || x < 0 || x >= mWidth)
    {
        return false;
    }
    return mTileData[y][x] == 0;
}

//------------------------------------------------------------------------------------------------------------------------

bool LevelManager::IsTileWalkablePlayer(int x, int y) const
{
    if (y < 0 || y >= mHeight || x < 0 || x >= mWidth)
    {
        return false;
    }
    return mTileData[y][x] == 0;
}

//------------------------------------------------------------------------------------------------------------------------

sf::Vector2f LevelManager::GetLevelCenterWorldPos() const
{
    return sf::Vector2f(
        (mWidth * mTileWidth) * 0.5f,
        (mHeight * mTileHeight) * 0.5f
    );
}

//------------------------------------------------------------------------------------------------------------------------

const LevelData & LevelManager::GetLevelData() const
{
    return mLevelData;
}

//------------------------------------------------------------------------------------------------------------------------

void LevelManager::ClearLevel()
{
    mTileLayers.clear();
    mTileData.clear();
}

//------------------------------------------------------------------------------------------------------------------------

void LevelManager::ParseTileData(const json & levelData)
{
    mTileLayers.clear();
    mTileData.clear();

    GameManager & gameManager = GetGameManager();

    if (!levelData.contains("levels") || !levelData["levels"].is_array() || levelData["levels"].empty())
    {
        std::cerr << "No levels found in the LDtk file." << std::endl;
        return;
    }

    const auto & level = levelData["levels"][0]; // Single level support for now

    if (!level.contains("pxWid") || !level.contains("pxHei"))
    {
        std::cerr << "Missing expected keys in level data." << std::endl;
        return;
    }

    if (!level.contains("layerInstances") || !level["layerInstances"].is_array() || level["layerInstances"].empty())
    {
        std::cerr << "No layerInstances found in level." << std::endl;
        return;
    }

    const auto & layerInstances = level["layerInstances"];
    const auto & firstLayer = layerInstances[0];

    if (!firstLayer.contains("__gridSize"))
    {
        std::cerr << "Missing __gridSize in first layer." << std::endl;
        return;
    }

    mTileWidth = firstLayer["__gridSize"].get<int>();
    mTileHeight = mTileWidth;
    mWidth = level["pxWid"].get<int>() / mTileWidth;
    mHeight = level["pxHei"].get<int>() / mTileHeight;

    mTileData.resize(mHeight, std::vector<int>(mWidth, 0));

    for (int i = int(layerInstances.size()) - 1; i >= 0; --i)
    {
        const auto & layer = layerInstances[i];

        if (!layer.contains("__type") || layer["__type"] != "Tiles")
        {
            continue;
        }

        if (!layer.contains("gridTiles"))
        {
            continue;
        }

        TileLayer tileLayer;
        tileLayer.name = layer["__identifier"];

        if (!layer.contains("__tilesetRelPath") || layer["__tilesetRelPath"].is_null())
        {
            std::cerr << "Layer missing or null tileset path: " << tileLayer.name << std::endl;
            continue;
        }

        std::string tilesetPath = layer["__tilesetRelPath"];
        auto resId = ResourceId(tilesetPath);
        tileLayer.texture = gameManager.GetManager<ResourceManager>()->GetTexture(resId);
        if (!tileLayer.texture)
        {
            std::cerr << "Failed to load texture for: " << tilesetPath << std::endl;
            continue;
        }

        int tileWidth = mTileWidth;
        int tileHeight = mTileHeight;
        const auto & tiles = layer["gridTiles"];
        tileLayer.vertices.setPrimitiveType(sf::Quads);
        tileLayer.vertices.resize(tiles.size() * 4);

        for (size_t t = 0; t < tiles.size(); ++t)
        {
            int px = tiles[t]["px"][0];
            int py = tiles[t]["px"][1];
            int srcX = tiles[t]["src"][0];
            int srcY = tiles[t]["src"][1];

            int cellX = px / tileWidth;
            int cellY = py / tileHeight;

            if (tileLayer.name == "Collision")
            {
                if (cellY >= 0 && cellY < mHeight && cellX >= 0 && cellX < mWidth)
                {
                    mTileData[cellY][cellX] = 1;

                    // Create a non-visible GameObject with collision only
                    BD::Handle handle = gameManager.CreateNewGameObject(ETeam::Neutral, gameManager.GetRootGameObjectHandle());
                    GameObject * pTileObj = gameManager.GetGameObject(handle);
                    if (!pTileObj)
                    {
                        continue;
                    }
                    sf::Vector2f tileCenter = sf::Vector2f(
                        float(px + tileWidth / 2),
                        float(py + tileHeight / 2)
                    );
                    pTileObj->SetPosition(tileCenter);

                    // Create static, non-sensor physics body
                    pTileObj->CreateBoxShapePhysicsBody(
                        &gameManager.GetPhysicsWorld(),
                        sf::Vector2f(float(tileWidth), float(tileHeight)),
                        false,                  // isDynamic = false
                        false                   // isSensor
                    );

                    auto pCollision = std::make_shared<CollisionComponent>(
                        pTileObj,
                        gameManager,
                        &gameManager.GetPhysicsWorld(),
                        pTileObj->GetPhysicsBody(),
                        sf::Vector2f(float(tileWidth), float(tileHeight)),
                        false // not a sensor
                    );
                    pTileObj->AddComponent(pCollision);
                }
            }

            sf::Vertex * quad = &tileLayer.vertices[t * 4];

            quad[0].position = sf::Vector2f(float(px), float(py));
            quad[1].position = sf::Vector2f(float(px + tileWidth), float(py));
            quad[2].position = sf::Vector2f(float(px + tileWidth), float(py + tileHeight));
            quad[3].position = sf::Vector2f(float(px), float(py + tileHeight));

            quad[0].texCoords = sf::Vector2f(float(srcX), float(srcY));
            quad[1].texCoords = sf::Vector2f(float(srcX + tileWidth), float(srcY));
            quad[2].texCoords = sf::Vector2f(float(srcX + tileWidth), float(srcY + tileHeight));
            quad[3].texCoords = sf::Vector2f(float(srcX), float(srcY + tileHeight));
        }

        mTileLayers.push_back(std::move(tileLayer));
    }

    // === ENTITY CREATION SUPPORT ===
    for (const auto & layer : layerInstances)
    {
        if (!layer.contains("entityInstances")) continue;

        const auto & entities = layer["entityInstances"];
        for (const auto & entity : entities)
        {
            std::string entityName = entity["__identifier"];
            int px = entity["px"][0];
            int py = entity["px"][1];

            BD::Handle objHandle = gameManager.CreateNewGameObject(ETeam::Neutral, gameManager.GetRootGameObjectHandle());
            GameObject * pObj = gameManager.GetGameObject(objHandle);
            if (pObj)
            {
                pObj->SetPosition(sf::Vector2f(float(px), float(py)));

                if (entityName == "Torch")
                {
                    auto pSpriteComponent = pObj->GetComponent<SpriteComponent>().lock();
                    if (pSpriteComponent)
                    {
                        ResourceId resId = ResourceId("../../VampireSurvivors/Art/Torch/TorchYellow.png");
                        auto pTexture = gameManager.GetManager<ResourceManager>()->GetTexture(resId);
                        if (pTexture)
                        {
                            pSpriteComponent->SetSprite(pTexture, sf::Vector2f(1.2f, 1.2f));
                            pSpriteComponent->GetSprite().setTextureRect(sf::IntRect(0, 0, 16, 28));
                            pSpriteComponent->GetSprite().setOrigin(8.f, 14.f);
                        }
                    }
                    auto pLightComponent = pObj->GetComponent<LightComponent>().lock();
                    if (!pLightComponent)
                    {
                        pLightComponent = std::make_shared<LightComponent>(pObj, gameManager, 200.f, sf::Color(255, 140, 0, 180));
                        pObj->AddComponent(pLightComponent);
                    }
                    CreateTorchAnimation(*pObj);
                }
                else if (entityName == "PlayerSpawnPosition")
                {
                    mLevelData.playerSpawnPosition = sf::Vector2f(float(px), float(py));
                }
                else if (entityName == "EnemySpawnPosition")
                {
                    mLevelData.enemySpawnPositions.emplace_back(sf::Vector2f(float(px), float(py)));
                }
                else if (entityName == "Door")
                {
                    auto pSpriteComponent = pObj->GetComponent<SpriteComponent>().lock();
                    if (pSpriteComponent)
                    {
                        ResourceId closedResId = ResourceId("../../VampireSurvivors/Art/Door/DoorClosed.png");
                        auto pTexture = gameManager.GetManager<ResourceManager>()->GetTexture(closedResId);
                        if (pTexture)
                        {
                            // Set the sprite to span the full door size
                            pSpriteComponent->SetSprite(pTexture);
                            pSpriteComponent->GetSprite().setTextureRect(sf::IntRect(0, 0, 32, 48)); // or whatever size
                        }
                    }

                    int price = -1;

                    if (entity.contains("fieldInstances"))
                    {
                        const auto & fields = entity["fieldInstances"];
                        for (const auto & field : fields)
                        {
                            if (field.contains("__identifier") && field["__identifier"] == "Price")
                            {
                                if (field.contains("__value") && field["__value"].is_number_integer())
                                {
                                    price = field["__value"];
                                }
                                break;
                            }
                        }
                    }

                    //Add a DoorComponent to track state
                    auto pDoorComp = std::make_shared<DoorComponent>(pObj, gameManager, price);
                    pObj->AddComponent(pDoorComp);

                    // Collision Component
                    auto pCollisionComponent = pObj->GetComponent<CollisionComponent>().lock();
                    if (!pCollisionComponent)
                    {
                        pObj->CreateBoxShapePhysicsBody(
                            &gameManager.GetPhysicsWorld(),
                            pObj->GetSize(),
                            false,                          // isDynamic (static)
                            false                           // isSensor
                        );

                        pObj->AddComponent(std::make_shared<CollisionComponent>(
                            pObj,
                            gameManager,
                            &gameManager.GetPhysicsWorld(),
                            pObj->GetPhysicsBody(),
                            pObj->GetSize(),
                            true
                        ));
                    }
                }
            }
        }
    }
}

//------------------------------------------------------------------------------------------------------------------------

void LevelManager::CreateTorchAnimation(GameObject & obj)
{
    GameManager & gameManager = GetGameManager();
    auto pAnimComponent = obj.GetComponent<SpriteAnimationComponent>().lock();
    if (!pAnimComponent)
    {
        pAnimComponent = std::make_shared<SpriteAnimationComponent>(&obj, gameManager);
        obj.AddComponent(pAnimComponent);

        // Create Start animation (Top row: y = 0)
        Animation startAnimation;
        startAnimation.frames = {
            sf::IntRect(0,   0, 16, 16), // Frame 0
            sf::IntRect(16,  0, 16, 16), // Frame 1
            sf::IntRect(32,  0, 16, 16), // Frame 2
            sf::IntRect(48,  0, 16, 16),  // Frame 3
            sf::IntRect(64,  0, 16, 16),  // Frame 4
            sf::IntRect(80,  0, 16, 16),  // Frame 5
            sf::IntRect(96,  0, 16, 16),  // Frame 6
            sf::IntRect(112,  0, 16, 16),  // Frame 7
        };
        startAnimation.frameTime = 0.2f;

        pAnimComponent->AddAnimation(EAnimationState::Start, startAnimation);
        pAnimComponent->PlayAnimation(EAnimationState::Start);
    }
}


//------------------------------------------------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------------------------------------------------