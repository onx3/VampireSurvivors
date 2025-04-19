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
    if (y < 0 || y >= mHeight || x < 0 || x >= mWidth)
        return false;

    int tile = mTileData[y][x];
    return (tile == 299);
}

//------------------------------------------------------------------------------------------------------------------------

bool LevelManager::IsTileWalkablePlayer(int x, int y) const
{
    if (y < 0 || y >= mHeight || x < 0 || x >= mWidth)
        return false;

    int tile = mTileData[y][x];
    //return (tile == 131 || tile == 1097);
    return (true);
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

    auto tilesetResourceId = ResourceId("Art/TileSet.png");
    auto waterResourceId = ResourceId("Art/Water.png");
    auto pTilesetTexture = resourceManager->GetTexture(tilesetResourceId);
    auto pWaterTexture = resourceManager->GetTexture(waterResourceId);
    if (!pTilesetTexture || !pWaterTexture)
    {
        std::cerr << "Failed to load tileset or water texture." << std::endl;
        return;
    }

    if (!levelData.contains("layers") || !levelData["layers"].is_array())
    {
        std::cerr << "No layers found in level file." << std::endl;
        return;
    }

    const uint32_t FLIPPED_HORIZONTALLY_FLAG = 0x80000000;
    const uint32_t FLIPPED_VERTICALLY_FLAG = 0x40000000;
    const uint32_t FLIPPED_DIAGONALLY_FLAG = 0x20000000;

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
                    uint32_t tileID = data[y * mWidth + x].get<uint32_t>();
                    int actualTileID = tileID & ~(FLIPPED_HORIZONTALLY_FLAG | FLIPPED_VERTICALLY_FLAG | FLIPPED_DIAGONALLY_FLAG);
                    mTileData[y][x] = actualTileID;

                    if (actualTileID == 0)
                        continue;

                    bool flipH = (tileID & FLIPPED_HORIZONTALLY_FLAG) != 0;
                    bool flipV = (tileID & FLIPPED_VERTICALLY_FLAG) != 0;
                    bool flipD = (tileID & FLIPPED_DIAGONALLY_FLAG) != 0;

                    std::shared_ptr<sf::Texture> texture = pTilesetTexture;
                    int columns = texture->getSize().x / mTileWidth;

                    int row = (actualTileID - 1) / columns;
                    int column = (actualTileID - 1) % columns;

                    // Override specific tiles with custom textures and coordinates
                    switch (actualTileID)
                    {
                        case (299): // Cracked ground
                        {
                            column = 1;
                            row = 4;
                            break;
                        }
                            
                        case 203:
                        {
                            [[fallthrough]];
                        }
                        case 35:
                        {
                            column = 2;
                            row = 1;
                            break;
                        }

                        case (1028): // Water tile from Water.png
                        {
                            texture = pWaterTexture;
                            columns = texture->getSize().x / mTileWidth;
                            column = 2;
                            row = 1;
                            break;
                        }

                        case (1097): // Ice Bridge tile from Water.png
                        {
                            texture = pWaterTexture;
                            columns = texture->getSize().x / mTileWidth;
                            column = 0;
                            row = 6;
                            break;
                        }

                        default:
                        {
                            int ii = 0;
                            ++ii;
                            break;
                        }
                    }

                    sf::Sprite sprite;
                    sprite.setTexture(*texture);
                    sprite.setTextureRect(sf::IntRect(column * mTileWidth, row * mTileHeight, mTileWidth, mTileHeight));
                    sprite.setOrigin(mTileWidth / 2.f, mTileHeight / 2.f);

                    float worldX = static_cast<float>(x * mTileWidth);
                    float worldY = static_cast<float>(y * mTileHeight);
                    sprite.setPosition(worldX + mTileWidth / 2.f, worldY + mTileHeight / 2.f);

                    if (flipD)
                    {
                        if (flipH && flipV)
                        {
                            sprite.setRotation(90.f);
                            sprite.scale(-1.f, 1.f);
                        }
                        else if (flipH)
                        {
                            sprite.setRotation(270.f);
                        }
                        else if (flipV)
                        {
                            sprite.setRotation(90.f);
                        }
                        else
                        {
                            sprite.setRotation(270.f);
                            sprite.scale(-1.f, 1.f);
                        }
                    }
                    else
                    {
                        sprite.setScale(flipH ? -1.f : 1.f, flipV ? -1.f : 1.f);
                    }

                    mTileSprites.push_back(sprite);
                }
            }
        }
    }
}


//------------------------------------------------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------------------------------------------------