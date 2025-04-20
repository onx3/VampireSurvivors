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
    , mTileVertices()
    , mWaterTileVertices()
    , mTilesetTexture()
    , mWaterTexture()
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
    if (mTilesetTexture)
    {
        window.draw(mTileVertices, mTilesetTexture.get());

    }
    if (mWaterTexture)
    {
        window.draw(mWaterTileVertices, mWaterTexture.get());
    }
}

//------------------------------------------------------------------------------------------------------------------------

bool LevelManager::IsTileWalkableAI(int x, int y) const
{
    if (y < 0 || y >= mHeight || x < 0 || x >= mWidth)
        return false;

    int tile = mTileData[y][x];
    return (tile == 131 || tile == 1097);
}

//------------------------------------------------------------------------------------------------------------------------

bool LevelManager::IsTileWalkablePlayer(int x, int y) const
{
    if (y < 0 || y >= mHeight || x < 0 || x >= mWidth)
        return false;

    int tile = mTileData[y][x];
    return (tile == 131 || tile == 1097);
}

//------------------------------------------------------------------------------------------------------------------------

void LevelManager::ClearLevel()
{
    mTileData.clear();

    mTileVertices.clear();
    mTileVertices.resize(0);

    mWaterTileVertices.clear();
    mWaterTileVertices.resize(0);
}

//------------------------------------------------------------------------------------------------------------------------

void LevelManager::ParseTileData(const json & levelData)
{
    mTileData.clear();
    mTileVertices.clear();
    mWaterTileVertices.clear();

    ResourceManager * resourceManager = GetGameManager().GetManager<ResourceManager>();

    auto tilesetResourceId = ResourceId("Art/TileSet.png");
    auto waterResourceId = ResourceId("Art/Water.png");

    mTilesetTexture = resourceManager->GetTexture(tilesetResourceId);
    mWaterTexture = resourceManager->GetTexture(waterResourceId);

    if (!mTilesetTexture || !mWaterTexture)
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
        if (layer["type"] != "tilelayer" || !layer.contains("data"))
            continue;

        const auto & data = layer["data"];
        if (!data.is_array())
            continue;

        mTileData.resize(mHeight, std::vector<int>(mWidth));
        mTileVertices.setPrimitiveType(sf::Quads);
        mWaterTileVertices.setPrimitiveType(sf::Quads);
        mTileVertices.resize(mWidth * mHeight * 4);
        mWaterTileVertices.resize(mWidth * mHeight * 4);

        for (int y = 0; y < mHeight; ++y)
        {
            for (int x = 0; x < mWidth; ++x)
            {
                int tileIndex = y * mWidth + x;
                uint32_t tileID = data[tileIndex].get<uint32_t>();

                int actualTileID = tileID & ~(FLIPPED_HORIZONTALLY_FLAG | FLIPPED_VERTICALLY_FLAG | FLIPPED_DIAGONALLY_FLAG);
                mTileData[y][x] = actualTileID;

                if (actualTileID == 0)
                    continue;

                bool flipH = (tileID & FLIPPED_HORIZONTALLY_FLAG) != 0;
                bool flipV = (tileID & FLIPPED_VERTICALLY_FLAG) != 0;
                bool flipD = (tileID & FLIPPED_DIAGONALLY_FLAG) != 0;

                std::shared_ptr<sf::Texture> texture = mTilesetTexture;
                sf::Vertex * quad = &mTileVertices[tileIndex * 4];
                int columns = texture->getSize().x / mTileWidth;

                int column = (actualTileID - 1) % columns;
                int row = (actualTileID - 1) / columns;

                // Optional overrides
                switch (actualTileID)
                {
                    case 299: // Cracked ground
                        column = 1; row = 4; break;

                    case 203:
                    case 35: // Wall
                        column = 2; row = 1; break;

                    case 1028: // Water
                        texture = mWaterTexture;
                        columns = texture->getSize().x / mTileWidth;
                        column = 2; row = 1;
                        quad = &mWaterTileVertices[tileIndex * 4];
                        break;

                    case 1097: // Ice bridge
                        texture = mWaterTexture;
                        columns = texture->getSize().x / mTileWidth;
                        column = 0; row = 6;
                        quad = &mWaterTileVertices[tileIndex * 4];
                        break;
                }

                float xPos = static_cast<float>(x * mTileWidth);
                float yPos = static_cast<float>(y * mTileHeight);

                quad[0].position = { xPos, yPos };
                quad[1].position = { xPos + mTileWidth, yPos };
                quad[2].position = { xPos + mTileWidth, yPos + mTileHeight };
                quad[3].position = { xPos, yPos + mTileHeight };

                sf::Vector2f texTopLeft = { static_cast<float>(column * mTileWidth), static_cast<float>(row * mTileHeight) };
                sf::Vector2f texBottomRight = texTopLeft + sf::Vector2f{ static_cast<float>(mTileWidth), static_cast<float>(mTileHeight) };

                quad[0].texCoords = texTopLeft;
                quad[1].texCoords = { texBottomRight.x, texTopLeft.y };
                quad[2].texCoords = texBottomRight;
                quad[3].texCoords = { texTopLeft.x, texBottomRight.y };
            }
        }
    }
}


//------------------------------------------------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------------------------------------------------