#include "AstroidsPrivate.h"
#include "LevelManager.h"
#include "ResourceManager.h"
#include <fstream>
#include <iostream>

LevelManager::LevelManager(GameManager * pGameManager)
    : BaseManager(pGameManager)
    , mWidth(0)
    , mHeight(0)
    , mTileWidth(0)
    , mTileHeight(0)
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

    for (int i = static_cast<int>(layerInstances.size()) - 1; i >= 0; --i)
    {
        const auto & layer = layerInstances[i];
        if (!layer.contains("gridTiles")) continue;

        TileLayer tileLayer;
        tileLayer.name = layer["__identifier"];

        if (!layer.contains("__tilesetRelPath"))
        {
            std::cerr << "Layer missing tileset path: " << tileLayer.name << std::endl;
            continue;
        }

        std::string tilesetPath = layer["__tilesetRelPath"];
        auto resId = ResourceId(tilesetPath);
        tileLayer.texture = GetGameManager().GetManager<ResourceManager>()->GetTexture(resId);
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
                    mTileData[cellY][cellX] = 1; // Mark unwalkable
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
}


//------------------------------------------------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------------------------------------------------