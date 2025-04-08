#include "AstroidsPrivate.h"
#include <queue>
#include <unordered_map>
#include "AIPathComponent.h"
#include "PlayerManager.h"
#include "imgui.h"
#include "LevelManager.h"
#include "CameraManager.h"

//------------------------------------------------------------------------------------------------------------------------

namespace
{
    static const int skSearchRadius = 5;
    static const float skUpdateInterval = 0.35f;
}

//------------------------------------------------------------------------------------------------------------------------

AIPathComponent::AIPathComponent(GameObject * pGameObject, GameManager & gameManager)
    : GameComponent(pGameObject, gameManager)
    , mName("AIPathComponent")
    , mPath()
    , mPathIndex(0)
    , mLastPlayerTile()
    , mStopDistance(0.f)
    , mMovementSpeed(200.f)
    , mTimeSinceLastPlayerMovement(2.f)
    , mPlayerPosition()
{

}

//------------------------------------------------------------------------------------------------------------------------

AIPathComponent::~AIPathComponent()
{

}

//------------------------------------------------------------------------------------------------------------------------

void AIPathComponent::Update(float deltaTime)
{
    GameManager & gameManager = GetGameManager();
    auto playerHandle = gameManager.GetManager<PlayerManager>()->GetPlayers()[0];
    auto * pPlayer = gameManager.GetGameObject(playerHandle);
    auto * pLevelManager = gameManager.GetManager<LevelManager>();

    if (!pPlayer || !pLevelManager)
    {
        return;
    }

    auto myPosition = GetGameObject().GetPosition();

    mTimeSinceLastPlayerMovement += deltaTime;
    if (mTimeSinceLastPlayerMovement >= skUpdateInterval)
    {
        mPlayerPosition = pPlayer->GetPosition();
        mTimeSinceLastPlayerMovement = 0.f;
    }

    float distanceSquared = (mPlayerPosition.x - myPosition.x) * (mPlayerPosition.x - myPosition.x) +
        (mPlayerPosition.y - myPosition.y) * (mPlayerPosition.y - myPosition.y);

    if (distanceSquared <= mStopDistance * mStopDistance)
    {
        return;
    }

    auto cellSize = BD::gsPixelCountCellSize;
    sf::Vector2i myTile(static_cast<int>(myPosition.x / cellSize), static_cast<int>(myPosition.y / cellSize));
    sf::Vector2i playerTile(static_cast<int>(mPlayerPosition.x / cellSize), static_cast<int>(mPlayerPosition.y / cellSize));

    sf::Vector2i goalTile = FindClosestWalkableTile(playerTile);

    if (playerTile != mLastPlayerTile || mPath.empty())
    {
        mPath = FindPath(myTile, goalTile);
        mPathIndex = 0;
        mLastPlayerTile = playerTile;
    }

    if (!mPath.empty() && mPathIndex < mPath.size())
    {
        sf::Vector2i nextTile = mPath[mPathIndex];
        sf::Vector2f targetPosition(
            nextTile.x * cellSize + cellSize / 2.0f,
            nextTile.y * cellSize + cellSize / 2.0f
        );

        sf::Vector2f direction = targetPosition - myPosition;
        float distance = std::sqrt(direction.x * direction.x + direction.y * direction.y);

        if (distance > 0.01f)
        {
            direction /= distance;
        }

        float movementSpeed = mMovementSpeed * deltaTime;
        sf::Vector2f newPosition = myPosition + (direction * movementSpeed);

        //GetGameObject().SetPosition(newPosition);

        float threshold = movementSpeed * 1.5f;
        if (distance < threshold)
        {
            ++mPathIndex;
        }
    }
}

//------------------------------------------------------------------------------------------------------------------------

void AIPathComponent::DebugImGuiComponentInfo()
{
	auto gameObjPos = GetGameObject().GetPosition();
	ImGui::Text("Position x,y: %.3f, %.3f", gameObjPos.x, gameObjPos.y);
}

//------------------------------------------------------------------------------------------------------------------------

std::string & AIPathComponent::GetClassName()
{
	return mName;
}

//------------------------------------------------------------------------------------------------------------------------

std::vector<sf::Vector2i> AIPathComponent::FindPath(sf::Vector2i start, sf::Vector2i goal)
{
    auto * pLevelManager = GetGameManager().GetManager<LevelManager>();
    std::vector<sf::Vector2i> path;
    if (!pLevelManager || !pLevelManager->IsTileWalkableAI(goal.x, goal.y))
    {
        return path;
    }

    auto heuristic = [](sf::Vector2i a, sf::Vector2i b) {
        return abs(a.x - b.x) + abs(a.y - b.y);
        };

    std::priority_queue<Node, std::vector<Node>, std::greater<Node>> openList;
    std::unordered_map<int, Node> allNodes;

    auto getKey = [](sf::Vector2i pos) { return pos.y * 1000 + pos.x; };

    Node startNode = { start, 0, heuristic(start, goal), nullptr };
    openList.push(startNode);
    allNodes[getKey(start)] = startNode;

    std::vector<sf::Vector2i> directions = { 
        {0, -1}, {0, 1}, {-1, 0}, {1, 0}, // Cardinal
        {-1, -1}, {1, -1}, {-1, 1}, {1, 1} // Diagonal
    }; 

    while (!openList.empty())
    {
        Node current = openList.top();
        openList.pop();

        if (current.position == goal)
        {
            Node * pathNode = &allNodes[getKey(goal)];
            while (pathNode)
            {
                path.push_back(pathNode->position);
                pathNode = pathNode->parent;
            }
            std::reverse(path.begin(), path.end());
            return path;
        }

        for (const auto & dir : directions)
        {
            sf::Vector2i neighborPos = { current.position.x + dir.x, current.position.y + dir.y };

            if (!pLevelManager->IsTileWalkableAI(neighborPos.x, neighborPos.y)) continue;

            int gCost = current.gCost + 1;
            int hCost = heuristic(neighborPos, goal);
            int key = getKey(neighborPos);

            if (allNodes.find(key) == allNodes.end() || gCost < allNodes[key].gCost)
            {
                Node neighborNode = { neighborPos, gCost, hCost, &allNodes[getKey(current.position)] };
                allNodes[key] = neighborNode;
                openList.push(neighborNode);
            }
        }
    }
    return path;
}

//------------------------------------------------------------------------------------------------------------------------

sf::Vector2i AIPathComponent::FindClosestWalkableTile(sf::Vector2i targetTile)
{
    auto * pLevelManager = GetGameManager().GetManager<LevelManager>();
    if (pLevelManager->IsTileWalkableAI(targetTile.x, targetTile.y))
    {
        return targetTile;
    }

    sf::Vector2i closestTile = targetTile;
    int closestDist = INT_MAX;

    for (int dx = -skSearchRadius; dx <= skSearchRadius; ++dx)
    {
        for (int dy = -skSearchRadius; dy <= skSearchRadius; ++dy)
        {
            sf::Vector2i checkTile = { targetTile.x + dx, targetTile.y + dy };
            if (pLevelManager->IsTileWalkableAI(checkTile.x, checkTile.y))
            {
                int dist = std::abs(dx) + std::abs(dy);

                if (dist < closestDist)
                {
                    closestDist = dist;
                    closestTile = checkTile;
                }
            }
        }
    }
    return closestTile;
}

//------------------------------------------------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------------------------------------------------