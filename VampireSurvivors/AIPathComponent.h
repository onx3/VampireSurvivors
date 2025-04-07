#pragma once
#include "GameComponent.h"

struct Node
{
	sf::Vector2i position;
	int gCost = 0;
	int hCost = 0;

	int fCost() const
	{
		return gCost + hCost;
	}

	Node * parent = nullptr;

	bool operator>(const Node & other) const
	{
		return fCost() > other.fCost();
	}
};

class AIPathComponent : public GameComponent
{
public:
	AIPathComponent(GameObject * pGameObject, GameManager & gameManager);
	~AIPathComponent();

	virtual void Update(float deltaTime) override;
	virtual void DebugImGuiComponentInfo() override;
	virtual std::string & GetClassName() override;

private:
	std::vector<sf::Vector2i> FindPath(sf::Vector2i start, sf::Vector2i goal);

	sf::Vector2i FindClosestWalkableTile(sf::Vector2i targetTile);

	std::string mName;
	std::vector<sf::Vector2i> mPath;
	size_t mPathIndex;
	sf::Vector2i mLastPlayerTile;
	float mStopDistance; // Used if you don't want the enemy to run into the player
	float mMovementSpeed;
	float mTimeSinceLastPlayerMovement;
	sf::Vector2f mPlayerPosition;
};

