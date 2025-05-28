#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include "BaseManager.h"
#include "SFML/Graphics.hpp"
#include "BaseManager.h"

struct TileLayer
{
	std::string name;
	sf::VertexArray vertices;
	std::shared_ptr<sf::Texture> texture;
};

struct RoomData
{
    std::string name;
    sf::FloatRect bounds;
    std::vector<sf::Vector2f> enemySpawnPositions;

    sf::Vector2f center;
    std::vector<size_t> neighborIndices;
};

struct LevelData
{
    sf::Vector2f playerSpawnPosition;
    std::vector<RoomData> rooms;
};

using json = nlohmann::json;
class LevelManager : public BaseManager
{
public:
	LevelManager(GameManager * pGameManager);
	~LevelManager();

	bool LoadLevel(const std::string & filePath);
	void ClearLevel();

	virtual void Render(sf::RenderWindow & window) override;

	bool IsTileWalkableAI(int x, int y) const;
	bool IsTileWalkablePlayer(int x, int y) const;

	sf::Vector2f GetLevelCenterWorldPos() const;
    const LevelData & GetLevelData() const;

    const RoomData * GetRoomAtPosition(const sf::Vector2f & pos) const;

private:
	void ParseTileData(const json & levelData);

    void CreateTorchAnimation(GameObject & pot);

	std::vector<std::vector<int>> mTileData;
	int mWidth;
	int mHeight;
	int mTileWidth;
	int mTileHeight;

	std::vector<TileLayer> mTileLayers;
	sf::VertexArray mWaterTileVertices;
	std::shared_ptr<sf::Texture> mTilesetTexture;
	std::shared_ptr<sf::Texture> mWaterTexture;

    LevelData mLevelData;
};

