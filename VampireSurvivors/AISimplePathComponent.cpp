#include "AstroidsPrivate.h"
#include "AISimplePathComponent.h"
#include "LevelManager.h"
#include <queue>
#include "EnemyMeleeAttackComponent.h"

namespace BD {
    struct Vec2iHash
    {
        std::size_t operator()(const sf::Vector2i & v) const
        {
            return std::hash<int>()(v.x) ^ (std::hash<int>()(v.y) << 1);
        }
    };
}

//------------------------------------------------------------------------------------------------------------------------

AISimplePathComponent::AISimplePathComponent(GameObject * pOwner, GameManager & gameManager, BD::Handle followHandle)
    : GameComponent(pOwner, gameManager)
    , mFollowHandle(followHandle)
    , mMoveSpeed(9.f)
    , mPath()
    , mPathRecalcInterval(1.f)
    , mName("AISimplePathComponent")
{
    mPathRecalcTimer = float(rand() % 1000) / 1000.f * mPathRecalcInterval;
}

//------------------------------------------------------------------------------------------------------------------------

AISimplePathComponent::~AISimplePathComponent()
{

}

//------------------------------------------------------------------------------------------------------------------------

bool AISimplePathComponent::HasLineOfSight(const sf::Vector2f & from, const sf::Vector2f & to)
{
    const int tileSize = 16; // Change if your tiles are a different size
    sf::Vector2i start = static_cast<sf::Vector2i>(from / static_cast<float>(tileSize));
    sf::Vector2i end = static_cast<sf::Vector2i>(to / static_cast<float>(tileSize));

    int dx = std::abs(end.x - start.x);
    int dy = std::abs(end.y - start.y);

    int sx = (start.x < end.x) ? 1 : -1;
    int sy = (start.y < end.y) ? 1 : -1;

    int err = dx - dy;

    auto pLevelManager = GetGameManager().GetManager<LevelManager>();
    if (!pLevelManager)
        return false;

    while (start != end)
    {
        sf::Vector2i tileCenter = start * tileSize + sf::Vector2i(tileSize / 2, tileSize / 2);
        sf::Vector2f worldPos(float(tileCenter.x), float(tileCenter.y));
        float cellSize = BD::gsPixelCountCellSize;
        if (!pLevelManager->IsTileWalkableAI(int(worldPos.x / cellSize), int(worldPos.y / cellSize)))
        {
            return false;
        }

        int e2 = 2 * err;
        if (e2 > -dy)
        {
            err -= dy;
            start.x += sx;
        }
        if (e2 < dx)
        {
            err += dx;
            start.y += sy;
        }
    }

    return true;
}

//------------------------------------------------------------------------------------------------------------------------

std::vector<sf::Vector2f> AISimplePathComponent::FindPath(const sf::Vector2f & from, const sf::Vector2f & to)
{
    const int tileSize = 16;
    sf::Vector2i start = static_cast<sf::Vector2i>(from / static_cast<float>(tileSize));
    sf::Vector2i goal = static_cast<sf::Vector2i>(to / static_cast<float>(tileSize));

    std::queue<sf::Vector2i> frontier;
    std::unordered_map<sf::Vector2i, sf::Vector2i, BD::Vec2iHash> cameFrom;

    frontier.push(start);
    cameFrom[start] = start;

    auto * pLevelManager = GetGameManager().GetManager<LevelManager>();
    if (!pLevelManager)
    {
        return {};
    }

    std::vector<sf::Vector2i> directions = {
        { 1, 0 }, { -1, 0 }, { 0, 1 }, { 0, -1 }
    };

    while (!frontier.empty())
    {
        auto current = frontier.front();
        frontier.pop();

        if (current == goal)
            break;

        for (auto & dir : directions)
        {
            sf::Vector2i next = current + dir;
            sf::Vector2f worldCheck(float(next.x * tileSize + tileSize / 2),
                float(next.y * tileSize + tileSize / 2));

            float cellSize = BD::gsPixelCountCellSize;
            if (pLevelManager->IsTileWalkableAI(int(worldCheck.x / cellSize), int(worldCheck.y / cellSize)) && cameFrom.find(next) == cameFrom.end())
            {
                frontier.push(next);
                cameFrom[next] = current;
            }
        }
    }

    // Reconstruct path
    std::vector<sf::Vector2f> path;
    if (cameFrom.find(goal) == cameFrom.end())
    {
        return path; // No path found
    }

    for (sf::Vector2i current = goal; current != start; current = cameFrom[current])
    {
        sf::Vector2f worldPos(static_cast<float>(current.x * tileSize + tileSize / 2),
            static_cast<float>(current.y * tileSize + tileSize / 2));
        path.push_back(worldPos);
    }

    std::reverse(path.begin(), path.end());
    return path;
}

//------------------------------------------------------------------------------------------------------------------------
#if 0
void AISimplePathComponent::Update(float deltaTime)
{
    // Don't move if attacking.
    GameObject & myGameObj = GetGameObject();
    auto pEnemyMeleeComponent = myGameObj.GetComponent<EnemyMeleeAttackComponent>().lock();
    if (pEnemyMeleeComponent)
    {
        if (pEnemyMeleeComponent->GetAttackState() == EAttackState::WindUp)
        {
            return;
        }
    }

    GameObject * pFollowObj = GetGameManager().GetGameObject(mFollowHandle);
    if (!pFollowObj)
    {
        return;
    }

    sf::Vector2f myPos = myGameObj.GetPosition();
    sf::Vector2f followPos = pFollowObj->GetPosition();

    if (HasLineOfSight(myPos, followPos))
    {
        sf::Vector2f direction = followPos - myPos;
        float distance = std::sqrt(direction.x * direction.x + direction.y * direction.y);

        if (distance > 0.f)
        {
            direction /= distance;

            auto pSpriteComponent = myGameObj.GetComponent<SpriteComponent>().lock();
            if (pSpriteComponent)
            {
                auto & sprite = pSpriteComponent->GetSprite();
                if (direction.x > 0.f)
                {
                    sprite.setScale(std::abs(sprite.getScale().x), sprite.getScale().y);
                }
                else if (direction.x < 0.f)
                {
                    sprite.setScale(-std::abs(sprite.getScale().x), sprite.getScale().y);
                }
            }

            sf::Vector2f movement = direction * mMoveSpeed * deltaTime;
            myGameObj.SetPosition(myPos + movement);
        }
    }
    else
    {
        // Pathfinding fallback
        mPathRecalcTimer += deltaTime;
        if (mPath.empty() || mPathRecalcTimer >= mPathRecalcInterval)
        {
            mPath = FindPath(myPos, followPos);
            mPathRecalcTimer = 0.f;
        }

        if (!mPath.empty())
        {
            sf::Vector2f target = mPath.front();
            sf::Vector2f dir = target - myPos;
            float dist = std::sqrt(dir.x * dir.x + dir.y * dir.y);

            if (dist > 1.f)
            {
                dir /= dist;

                auto pSpriteComponent = myGameObj.GetComponent<SpriteComponent>().lock();
                if (pSpriteComponent)
                {
                    auto & sprite = pSpriteComponent->GetSprite();
                    if (dir.x > 0.f)
                    {
                        sprite.setScale(std::abs(sprite.getScale().x), sprite.getScale().y);
                    }
                    else if (dir.x < 0.f)
                    {
                        sprite.setScale(-std::abs(sprite.getScale().x), sprite.getScale().y);
                    }
                }

                sf::Vector2f movement = dir * mMoveSpeed * deltaTime;
                myGameObj.SetPosition(myPos + movement);
            }
            else
            {
                mPath.erase(mPath.begin()); // Reached this waypoint
            }
        }
    }
}
#endif

void AISimplePathComponent::Update(float deltaTime)
{
    GameObject & myGameObj = GetGameObject();
    b2Body * pBody = myGameObj.GetPhysicsBody();
    if (!pBody || !myGameObj.IsActive())
    {
        return;
    }

    // Don't move if attacking
    auto pEnemyMeleeComponent = myGameObj.GetComponent<EnemyMeleeAttackComponent>().lock();
    if (pEnemyMeleeComponent && pEnemyMeleeComponent->GetAttackState() == EAttackState::WindUp)
    {
        pBody->SetLinearVelocity(b2Vec2_zero);
        return;
    }

    GameObject * pFollowObj = GetGameManager().GetGameObject(mFollowHandle);
    if (!pFollowObj || !pFollowObj->IsActive())
    {
        pBody->SetLinearVelocity(b2Vec2_zero);
        return;
    }

    const sf::Vector2f & myPos = myGameObj.GetPosition();
    const sf::Vector2f & targetPos = pFollowObj->GetPosition();
    sf::Vector2f direction = targetPos - myPos;

    float distance = std::sqrt(direction.x * direction.x + direction.y * direction.y);

    // Stop close to the player to avoid jitter
    float stopDistance = 5.f;
    if (distance < stopDistance)
    {
        pBody->SetLinearVelocity(b2Vec2_zero);
        return;
    }

    // Normalize direction
    direction /= distance;

    auto pSpriteComponent = myGameObj.GetComponent<SpriteComponent>().lock();
    if (pSpriteComponent)
    {
        auto & sprite = pSpriteComponent->GetSprite();
        if (direction.x > 0.f)
        {
            sprite.setScale(std::abs(sprite.getScale().x), sprite.getScale().y);
        }
        else if (direction.x < 0.f)
        {
            sprite.setScale(-std::abs(sprite.getScale().x), sprite.getScale().y);
        }
    }

    b2Vec2 velocity(direction.x * mMoveSpeed / myGameObj.PIXELS_PER_METER, direction.y * mMoveSpeed / myGameObj.PIXELS_PER_METER);
    pBody->SetLinearVelocity(velocity);
}

//------------------------------------------------------------------------------------------------------------------------

void AISimplePathComponent::DebugImGuiComponentInfo()
{

}

//------------------------------------------------------------------------------------------------------------------------

const std::string & AISimplePathComponent::GetClassName()
{
    return mName;
}

//------------------------------------------------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------------------------------------------------