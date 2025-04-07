#include "AstroidsPrivate.h"
#include "LevelManager.h"
#include "ResourceManager.h"
#include <fstream>
#include <iostream>

LevelManager::LevelManager(GameManager * pGameManager)
	: BaseManager(pGameManager)
	, mTileData()
	, mWidth(0)
	, mHeight(0)
	, mTileWidth(0)
	, mTileHeight(0)
	, mTileSprites()
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
		printf("Failed to open file %s, is already open.", filePath.c_str());
		return false;
	}

	json levelData;
	file >> levelData;

	mWidth = levelData["width"];
	mHeight = levelData["height"];
	mTileWidth = levelData["tilewidth"];
	mTileHeight = levelData["tileheight"];

	ParseTileData(levelData);
	return true;
}

//------------------------------------------------------------------------------------------------------------------------

void LevelManager::Render(sf::RenderWindow & window)
{
	for (const auto & sprite : mTileSprites)
	{
		window.draw(sprite);
	}
}

//------------------------------------------------------------------------------------------------------------------------

bool LevelManager::IsTileWalkableAI(int x, int y) const
{
#if 0
    if (y < 0 || y >= mHeight || x < 0 || x >= mWidth)
        return false;

    int tile = mTileData[y][x];
    return (tile == 70);
#endif
    return true;
}

//------------------------------------------------------------------------------------------------------------------------

bool LevelManager::IsTileWalkablePlayer(int x, int y) const
{
#if 0
    if (y < 0 || y >= mHeight || x < 0 || x >= mWidth)
        return false;

    int tile = mTileData[y][x];
    return (tile == 7 || tile == 70);
#endif
    return true;
}

//------------------------------------------------------------------------------------------------------------------------

void LevelManager::ClearLevel()
{
	mTileData.clear();
	mTileSprites.clear();
}

//------------------------------------------------------------------------------------------------------------------------

void LevelManager::ParseTileData(const json & levelData)
{
    mTileSprites.clear();
    mTileData.clear();

    ResourceManager * resourceManager = GetGameManager().GetManager<ResourceManager>();
    auto resourceID = ResourceId("Art/TileSet.png");
    auto tilesetTexture = resourceManager->GetTexture(resourceID);
    if (!tilesetTexture)
    {
        std::cerr << "Failed to load tileset texture." << std::endl;
        return;
    }

    if (!levelData.contains("layers") || !levelData["layers"].is_array())
    {
        std::cerr << "No layers found in level file." << std::endl;
        return;
    }

    for (const auto & layer : levelData["layers"])
    {
        if (layer["type"] == "tilelayer" && layer.contains("data"))
        {
            const auto & data = layer["data"];
            if (!data.is_array())
            {
                std::cerr << "Invalid layer data." << std::endl;
                return;
            }

            mTileData.resize(mHeight, std::vector<int>(mWidth));

            for (int y = 0; y < mHeight; ++y)
            {
                for (int x = 0; x < mWidth; ++x)
                {
                    int tileID = data[y * mWidth + x].get<int>();

                    // Extract transformation flags
                    int actualTileID = tileID & 0x1FFFFFFF;
                    bool flippedHorizontally = (tileID & 0x80000000) != 0;
                    bool flippedVertically = (tileID & 0x40000000) != 0;
                    bool flippedDiagonally = (tileID & 0x20000000) != 0;

                    mTileData[y][x] = actualTileID;

                    if (actualTileID > 0)
                    {
                        sf::Sprite sprite;
                        sprite.setTexture(*tilesetTexture);

                        int columns = tilesetTexture->getSize().x / mTileWidth;
                        int row = (actualTileID - 1) / columns;
                        int column = (actualTileID - 1) % columns;

                        sprite.setTextureRect(sf::IntRect(column * mTileWidth, row * mTileHeight, mTileWidth, mTileHeight));

                        float worldX = static_cast<float>(x * mTileWidth);
                        float worldY = static_cast<float>(y * mTileHeight);
                        sprite.setPosition(worldX, worldY);

                        // Apply transformations
                        sf::Vector2f origin(mTileWidth / 2.0f, mTileHeight / 2.0f);
                        sprite.setOrigin(origin);

                        if (flippedHorizontally)
                            sprite.scale(-1.f, 1.f); // Flip horizontally
                        if (flippedVertically)
                            sprite.scale(1.f, -1.f); // Flip vertically
                        if (flippedDiagonally)
                            sprite.setRotation(90.f); // 90-degree diagonal flip

                        mTileSprites.push_back(sprite);
                    }
                }
            }
        }
    }
}

//------------------------------------------------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------------------------------------------------