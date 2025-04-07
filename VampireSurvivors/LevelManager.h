#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include "BaseManager.h"
#include "SFML/Graphics.hpp"
#include "BaseManager.h"

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

private:
	void ParseTileData(const json & levelData);

	std::vector<std::vector<int>> mTileData;
	int mWidth;
	int mHeight;
	int mTileWidth;
	int mTileHeight;

	std::vector<sf::Sprite> mTileSprites;
};

