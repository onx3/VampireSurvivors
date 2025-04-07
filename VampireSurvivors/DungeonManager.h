#pragma once
#include <vector>
#include <unordered_map>
#include <string>
#include "BaseManager.h"

enum class EDungeonPiece : unsigned char
{
    Empty,
    Water,
    Path,
    Brick
};

//------------------------------------------------------------------------------------------------------------------------

struct Room
{
    int x;
    int y;
    int width;
    int height;
    int centerX;
    int centerY;

    Room(int x, int y, int width, int height);
};

//------------------------------------------------------------------------------------------------------------------------

struct Connection
{
    int roomA;
    int roomB;
    float distance;

    Connection(int a, int b, float dist);

    bool operator<(const Connection & other) const
    {
        return distance < other.distance;
    }
};

//------------------------------------------------------------------------------------------------------------------------

class BaseManager;
class DungeonManager : public BaseManager
{
public:

    DungeonManager(GameManager * pGameManager, int roomCount, int gridWidth, int gridHeight, int minSize, int maxSize);

    const std::vector<std::vector<EDungeonPiece>> & GetDungeonGrid() const;

private:
    
    //void GenerateRooms();

    float CalculateDistance(int x1, int y1, int x2, int y2);

    void GenerateRooms();

    void GenerateConnections();

    void CreatePath(int startX, int startY, int endX, int endY);

    bool IsValidPlacement(int x, int y, EDungeonPiece type) const;

    int mRoomCount;
    int mGridWidth;
    int mGridHeight;
    int mRoomMinSize;
    int mRoomMaxSize;

    std::unordered_map<EDungeonPiece, std::vector<EDungeonPiece>> mNeighborRules;
    std::vector<std::vector<EDungeonPiece>> mDungeonGrid;

    std::vector<Room> mRooms;
    std::vector<Connection> mConnections;
};




//class BaseManager;
//class DungeonManager : public BaseManager
//{
//public:
//    DungeonManager(GameManager * pGameManager, int width, int height);
//
//    const std::vector<std::vector<EDungeonPiece>> & GetDungeonGrid() const;
//
//    void Update(float deltaTime) override;
//
//    void GenerateDungeonGrid();
//
//    bool IsTileWalkable(EDungeonPiece tile);
//
//private:
//    bool CanPlaceTile(int x, int y, EDungeonPiece type) const;
//
//    // Perlin Noise Helpers
//    float Lerp(float a, float b, float t);
//    float Fade(float t);
//    int Hash(int x, int y, int seed);
//    float Gradient(int x, int y, int seed);
//    float Perlin(float x, float y, int seed);
//
//    void ApplyCellularAutomata();
//
//    void EnsureConnectivity();
//
//    void FloodFill(int startX, int startY, std::vector<std::vector<bool>> & visited, std::vector<std::pair<int, int>> & cluster);
//
//    void ConnectClusters(const std::vector<std::pair<int, int>> & clusterA, const std::vector<std::pair<int, int>> & clusterB);
//
//    void CreatePath(int startX, int startY, int endX, int endY);
//
//    bool IsWithinBounds(int x, int y);
//
//    int Heuristic(int x1, int y1, int x2, int y2);
//
//
//    int mWidth;
//    int mHeight;
//    float mTimeSinceLastStep;
//    float mStepDelay;
//    int mCurrentStep;
//    std::unordered_map<EDungeonPiece, std::vector<EDungeonPiece>> mNeighborRules;
//    std::vector<std::vector<EDungeonPiece>> mDungeonGrid;
//};