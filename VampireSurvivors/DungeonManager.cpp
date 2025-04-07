#include "AstroidsPrivate.h"
#include "DungeonManager.h"
#include <unordered_map>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <queue>
#include <climits>
#include <unordered_set>
#include <numeric>

//------------------------------------------------------------------------------------------------------------------------
// Room
//------------------------------------------------------------------------------------------------------------------------

Room::Room(int x, int y, int width, int height)
    : x(x)
    , y(y)
    , width(width)
    , height(height)
    , centerX(x + width / 2)
    , centerY(y + height / 2)
{
}

//------------------------------------------------------------------------------------------------------------------------
// Connection
//------------------------------------------------------------------------------------------------------------------------

Connection::Connection(int a, int b, float dist)
    : roomA(a)
    , roomB(b)
    , distance(dist)
{
}

//------------------------------------------------------------------------------------------------------------------------
// DungeonManager
//------------------------------------------------------------------------------------------------------------------------

DungeonManager::DungeonManager(GameManager * pGameManager, int roomCount, int gridWidth, int gridHeight, int minSize, int maxSize)
    : BaseManager(pGameManager)
    , mRoomCount(roomCount)
    , mGridWidth(gridWidth)
    , mGridHeight(gridHeight)
    , mRoomMinSize(minSize)
    , mRoomMaxSize(maxSize)
    , mDungeonGrid()
{
    mNeighborRules = {
        { EDungeonPiece::Water, { EDungeonPiece::Water, EDungeonPiece::Brick, EDungeonPiece::Path, EDungeonPiece::Empty } },
        { EDungeonPiece::Brick, { EDungeonPiece::Brick, EDungeonPiece::Water, EDungeonPiece::Path, EDungeonPiece::Empty } },
        { EDungeonPiece::Path,  { EDungeonPiece::Brick, EDungeonPiece::Water, EDungeonPiece::Path, EDungeonPiece::Empty } }
        };
    GenerateRooms();
    GenerateConnections();
}

//------------------------------------------------------------------------------------------------------------------------

float DungeonManager::CalculateDistance(int x1, int y1, int x2, int y2)
{
    return float(std::sqrt(std::pow(x2 - x1, 2) + std::pow(y2 - y1, 2)));
}

//------------------------------------------------------------------------------------------------------------------------

void DungeonManager::GenerateRooms()
{
    mRooms.clear();
    mDungeonGrid.assign(mGridHeight, std::vector<EDungeonPiece>(mGridWidth, EDungeonPiece::Empty));
    srand(static_cast<unsigned>(time(nullptr)));

    for (int i = 0; i < mRoomCount; ++i)
    {
        int width = rand() % (mRoomMaxSize - mRoomMinSize + 1) + mRoomMinSize;
        int height = rand() % (mRoomMaxSize - mRoomMinSize + 1) + mRoomMinSize;
        int x = rand() % (mGridWidth - width);
        int y = rand() % (mGridHeight - height);

        Room newRoom(x, y, width, height);

        for (int row = y - 1; row <= y + height; ++row)
        {
            for (int col = x - 1; col <= x + width; ++col)
            {
                if (row >= 0 && row < mGridHeight && col >= 0 && col < mGridWidth)
                {
                    if (row == y - 1 || row == y + height || col == x - 1 || col == x + width)
                    {
                        if (mDungeonGrid[row][col] == EDungeonPiece::Empty)
                        {
                            mDungeonGrid[row][col] = EDungeonPiece::Water;
                        }
                    }
                    else
                    {
                        mDungeonGrid[row][col] = EDungeonPiece::Brick;
                    }
                }
            }
        }

        mRooms.push_back(newRoom);
    }
}

//------------------------------------------------------------------------------------------------------------------------

void DungeonManager::GenerateConnections()
{
    mConnections.clear();
    std::vector<Connection> allConnections;

    // Create all possible connections
    for (size_t i = 0; i < mRooms.size(); ++i)
    {
        for (size_t j = i + 1; j < mRooms.size(); ++j)
        {
            float distance = CalculateDistance(mRooms[i].centerX, mRooms[i].centerY,
                mRooms[j].centerX, mRooms[j].centerY);
            allConnections.emplace_back(int(i), int(j), distance);
        }
    }

    // Sort connections by distance
    std::sort(allConnections.begin(), allConnections.end());

    // Use MST (Kruskal's Algorithm) to ensure connectivity
    std::vector<int> parent(mRooms.size());
    std::iota(parent.begin(), parent.end(), 0); // Initialize disjoint set

    auto find = [&](int node) {
        while (node != parent[node])
        {
            parent[node] = parent[parent[node]]; // Path compression
            node = parent[node];
        }
        return node;
        };

    auto unite = [&](int a, int b) {
        parent[find(a)] = find(b);
        };

    for (const auto & conn : allConnections)
    {
        if (find(conn.roomA) != find(conn.roomB))
        {
            mConnections.push_back(conn);

            // Carve a path between the connected rooms
            CreatePath(mRooms[conn.roomA].centerX, mRooms[conn.roomA].centerY,
                mRooms[conn.roomB].centerX, mRooms[conn.roomB].centerY);

            unite(conn.roomA, conn.roomB);
        }
    }
}

//------------------------------------------------------------------------------------------------------------------------

void DungeonManager::CreatePath(int startX, int startY, int endX, int endY)
{
    int x = startX;
    int y = startY;

    // Horizontal path
    while (x != endX)
    {
        mDungeonGrid[y][x] = EDungeonPiece::Path;
        x += (endX > x) ? 1 : -1;
    }

    // Vertical path
    while (y != endY)
    {
        mDungeonGrid[y][x] = EDungeonPiece::Path;
        y += (endY > y) ? 1 : -1;
    }
}

//------------------------------------------------------------------------------------------------------------------------

bool DungeonManager::IsValidPlacement(int x, int y, EDungeonPiece type) const
{
    if (x < 0 || x >= mGridWidth || y < 0 || y >= mGridHeight) return false;

    const auto & validNeighbors = mNeighborRules.at(type);

    // Check neighbors (up, right, down, left)
    static const std::vector<std::pair<int, int>> directions = {
        {0, -1}, {1, 0}, {0, 1}, {-1, 0}
    };

    for (const auto & dir : directions)
    {
        int nx = x + dir.first;
        int ny = y + dir.second;

        // Skip out-of-bounds neighbors
        if (nx < 0 || nx >= mGridWidth || ny < 0 || ny >= mGridHeight) continue;

        EDungeonPiece neighbor = mDungeonGrid[ny][nx];
        if (std::find(validNeighbors.begin(), validNeighbors.end(), neighbor) == validNeighbors.end())
        {
            return false;
        }
    }

    return true;
}

const std::vector<std::vector<EDungeonPiece>> & DungeonManager::GetDungeonGrid() const
{
    return mDungeonGrid;
}




//DungeonManager::DungeonManager(GameManager * pGameManager, int width, int height)
//    : BaseManager(pGameManager)
//    , mWidth(width)
//    , mHeight(height)
//    , mTimeSinceLastStep(0.f)
//    , mStepDelay(0.001f)
//    , mCurrentStep(0)
//    , mDungeonGrid(mHeight, std::vector<EDungeonPiece>(mWidth, EDungeonPiece::Empty))
//{
//    std::srand(static_cast<unsigned>(std::time(nullptr)));
//
//    mNeighborRules = {
//    { EDungeonPiece::Water, { EDungeonPiece::Water, EDungeonPiece::Brick, EDungeonPiece::Path, EDungeonPiece::Empty } },
//    { EDungeonPiece::Brick, { EDungeonPiece::Brick, EDungeonPiece::Water, EDungeonPiece::Path, EDungeonPiece::Empty } },
//    { EDungeonPiece::Path,  { EDungeonPiece::Brick, EDungeonPiece::Water, EDungeonPiece::Path, EDungeonPiece::Empty } }
//    };
//}
//
////------------------------------------------------------------------------------------------------------------------------
//
//const std::vector<std::vector<EDungeonPiece>> & DungeonManager::GetDungeonGrid() const
//{
//    return mDungeonGrid;
//}
//
////------------------------------------------------------------------------------------------------------------------------
//
//void DungeonManager::Update(float deltaTime)
//{
//    // Implement any runtime updates if necessary
//}
//
////------------------------------------------------------------------------------------------------------------------------
//
//void DungeonManager::GenerateDungeonGrid()
//{
//    for (int y = 0; y < mHeight; ++y)
//    {
//        for (int x = 0; x < mWidth; ++x)
//        {
//            mDungeonGrid[y][x] = (std::rand() % 100 < 45) ? EDungeonPiece::Water : EDungeonPiece::Brick;
//        }
//    }
//
//    ApplyCellularAutomata();
//    EnsureConnectivity();
//}
//
////------------------------------------------------------------------------------------------------------------------------
//
//bool DungeonManager::IsTileWalkable(EDungeonPiece tile)
//{
//    return true;
//    //return tile == EDungeonPiece::Brick || tile == EDungeonPiece::Path;
//}
//
////------------------------------------------------------------------------------------------------------------------------
//
//bool DungeonManager::CanPlaceTile(int x, int y, EDungeonPiece type) const
//{
//    if (x < 0 || x >= mWidth || y < 0 || y >= mHeight) return false;
//
//    const auto & validNeighbors = mNeighborRules.at(type);
//
//    // Check direct neighbors
//    if (y > 0 && std::find(validNeighbors.begin(), validNeighbors.end(), mDungeonGrid[y - 1][x]) == validNeighbors.end()) return false; // Up
//    if (x < mWidth - 1 && std::find(validNeighbors.begin(), validNeighbors.end(), mDungeonGrid[y][x + 1]) == validNeighbors.end()) return false; // Right
//    if (y < mHeight - 1 && std::find(validNeighbors.begin(), validNeighbors.end(), mDungeonGrid[y + 1][x]) == validNeighbors.end()) return false; // Down
//    if (x > 0 && std::find(validNeighbors.begin(), validNeighbors.end(), mDungeonGrid[y][x - 1]) == validNeighbors.end()) return false; // Left
//
//    if (x > 0 && y > 0 && std::find(validNeighbors.begin(), validNeighbors.end(), mDungeonGrid[y - 1][x - 1]) == validNeighbors.end()) return false; // Up-Left
//    if (x < mWidth - 1 && y > 0 && std::find(validNeighbors.begin(), validNeighbors.end(), mDungeonGrid[y - 1][x + 1]) == validNeighbors.end()) return false; // Up-Right
//    if (x > 0 && y < mHeight - 1 && std::find(validNeighbors.begin(), validNeighbors.end(), mDungeonGrid[y + 1][x - 1]) == validNeighbors.end()) return false; // Down-Left
//    if (x < mWidth - 1 && y < mHeight - 1 && std::find(validNeighbors.begin(), validNeighbors.end(), mDungeonGrid[y + 1][x + 1]) == validNeighbors.end()) return false; // Down-Right
//
//    return true;
//}
//
////------------------------------------------------------------------------------------------------------------------------
//
//void DungeonManager::ApplyCellularAutomata()
//{
//    for (int step = 0; step < 4; ++step)
//    {
//        std::vector<std::vector<EDungeonPiece>> newGrid = mDungeonGrid;
//
//        for (int y = 1; y < mHeight - 1; ++y)
//        {
//            for (int x = 1; x < mWidth - 1; ++x)
//            {
//                int waterCount = 0;
//
//                for (int dy = -1; dy <= 1; ++dy)
//                {
//                    for (int dx = -1; dx <= 1; ++dx)
//                    {
//                        if (dy != 0 || dx != 0)
//                        {
//                            if (mDungeonGrid[y + dy][x + dx] == EDungeonPiece::Water)
//                                ++waterCount;
//                        }
//                    }
//                }
//
//                if (mDungeonGrid[y][x] == EDungeonPiece::Water)
//                {
//                    newGrid[y][x] = (waterCount >= 4) ? EDungeonPiece::Water : EDungeonPiece::Brick;
//                }
//                else
//                {
//                    newGrid[y][x] = (waterCount >= 5) ? EDungeonPiece::Water : EDungeonPiece::Brick;
//                }
//            }
//        }
//
//        mDungeonGrid = newGrid;
//    }
//}
//
////------------------------------------------------------------------------------------------------------------------------
//
//void DungeonManager::EnsureConnectivity()
//{
//    std::vector<std::vector<bool>> visited(mHeight, std::vector<bool>(mWidth, false));
//    std::vector<std::vector<std::pair<int, int>>> clusters;
//
//    for (int y = 0; y < mHeight; ++y)
//    {
//        for (int x = 0; x < mWidth; ++x)
//        {
//            if (!visited[y][x] && IsTileWalkable(mDungeonGrid[y][x]))
//            {
//                std::vector<std::pair<int, int>> cluster;
//                FloodFill(x, y, visited, cluster);
//                clusters.push_back(cluster);
//            }
//        }
//    }
//
//    for (size_t i = 1; i < clusters.size(); ++i)
//    {
//        ConnectClusters(clusters[0], clusters[i]);
//    }
//}
//
////------------------------------------------------------------------------------------------------------------------------
//
//void DungeonManager::FloodFill(int startX, int startY, std::vector<std::vector<bool>> & visited, std::vector<std::pair<int, int>> & cluster)
//{
//    std::queue<std::pair<int, int>> queue;
//    queue.push({ startX, startY });
//    visited[startY][startX] = true;
//
//    while (!queue.empty())
//    {
//        auto [x, y] = queue.front();
//        queue.pop();
//        cluster.push_back({ x, y });
//
//        for (int dy = -1; dy <= 1; ++dy)
//        {
//            for (int dx = -1; dx <= 1; ++dx)
//            {
//                if (std::abs(dx) + std::abs(dy) == 1)
//                {
//                    int nx = x + dx, ny = y + dy;
//                    if (IsWithinBounds(nx, ny) && !visited[ny][nx] && IsTileWalkable(mDungeonGrid[ny][nx]))
//                    {
//                        visited[ny][nx] = true;
//                        queue.push({ nx, ny });
//                    }
//                }
//            }
//        }
//    }
//}
//
////------------------------------------------------------------------------------------------------------------------------
//
//void DungeonManager::ConnectClusters(const std::vector<std::pair<int, int>> & clusterA, const std::vector<std::pair<int, int>> & clusterB)
//{
//    int bestDist = INT_MAX;
//    std::pair<int, int> bestA, bestB;
//
//    for (const auto & tileA : clusterA)
//    {
//        for (const auto & tileB : clusterB)
//        {
//            int dist = Heuristic(tileA.first, tileA.second, tileB.first, tileB.second);
//            if (dist < bestDist)
//            {
//                bestDist = dist;
//                bestA = tileA;
//                bestB = tileB;
//            }
//        }
//    }
//
//    CreatePath(bestA.first, bestA.second, bestB.first, bestB.second);
//}
//
////------------------------------------------------------------------------------------------------------------------------
//
//void DungeonManager::CreatePath(int startX, int startY, int endX, int endY)
//{
//    struct Node
//    {
//        int x, y, g, h;
//        Node * parent;
//        int F() const
//        {
//            return g + h;
//        }
//    };
//
//    auto CompareNodes = [](Node * a, Node * b) { return a->F() > b->F(); };
//
//    std::priority_queue<Node *, std::vector<Node *>, decltype(CompareNodes)> openList(CompareNodes);
//    std::vector<std::vector<bool>> closedList(mHeight, std::vector<bool>(mWidth, false));
//    std::unordered_set<int> inOpenList; // Tracks nodes in openList
//
//    // Lambda to generate unique keys for nodes (x, y)
//    auto NodeKey = [this](int x, int y) { return y * mWidth + x; };
//
//    openList.push(new Node{ startX, startY, 0, Heuristic(startX, startY, endX, endY), nullptr });
//    inOpenList.insert(NodeKey(startX, startY));
//
//    while (!openList.empty())
//    {
//        Node * current = openList.top();
//        openList.pop();
//        inOpenList.erase(NodeKey(current->x, current->y));
//
//        if (current->x == endX && current->y == endY)
//        {
//            while (current)
//            {
//                // Set path tiles
//                if (mDungeonGrid[current->y][current->x] == EDungeonPiece::Brick)
//                {
//                    mDungeonGrid[current->y][current->x] = EDungeonPiece::Path;
//                }
//                current = current->parent;
//            }
//            return;
//        }
//
//        closedList[current->y][current->x] = true;
//
//        for (int dy = -1; dy <= 1; ++dy)
//        {
//            for (int dx = -1; dx <= 1; ++dx)
//            {
//                if (std::abs(dx) + std::abs(dy) == 1) // Cardinal directions only
//                {
//                    int nx = current->x + dx, ny = current->y + dy;
//
//                    // Skip if out of bounds or already processed
//                    if (!IsWithinBounds(nx, ny) || closedList[ny][nx] || !IsTileWalkable(mDungeonGrid[ny][nx]))
//                        continue;
//
//                    // Skip if already in openList
//                    if (inOpenList.find(NodeKey(nx, ny)) != inOpenList.end())
//                        continue;
//
//                    openList.push(new Node{ nx, ny, current->g + 1, Heuristic(nx, ny, endX, endY), current });
//                    inOpenList.insert(NodeKey(nx, ny));
//                }
//            }
//        }
//    }
//}
//
////------------------------------------------------------------------------------------------------------------------------
//
//bool DungeonManager::IsWithinBounds(int x, int y)
//{
//    return x >= 0 && x < mWidth && y >= 0 && y < mHeight;
//}
//
////------------------------------------------------------------------------------------------------------------------------
//
//int DungeonManager::Heuristic(int x1, int y1, int x2, int y2)
//{
//    return std::abs(x1 - x2) + std::abs(y1 - y2); // Manhattan distance
//}

//------------------------------------------------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------------------------------------------------
