#pragma once
#include "SFML/Graphics.hpp"
#include <unordered_map>
#include <typeindex>
#include <memory>
#include <vector>
#include <string>
#include "box2d/box2d.h"
#include "TPool.h"

class GameComponent;
class GameManager;

enum class ETeam
{
    Player,
    FriendlyPersistant,
    FriendlyFleeting,
    Enemy,
    Neutral,
    CoinDrop,
};

enum ECollisionCategory : uint16
{
    CATEGORY_NONE = 0x0000,
    CATEGORY_PLAYER = 0x0001,
    CATEGORY_FRIENDLY = 0x0002,
    CATEGORY_ENEMY = 0x0004,
    CATEGORY_NEUTRAL = 0x0008,
    CATEGORY_PICKUP = 0x0020,
    CATEGORY_PROJECTILE = 0x0040,
};

struct CollisionRule
{
    uint16_t categoryBits;
    uint16_t maskBits;
};

inline CollisionRule GetCollisionRuleForTeam(ETeam team)
{
    switch (team)
    {
        case ETeam::Player:
        {
            return { CATEGORY_PLAYER, CATEGORY_NEUTRAL | CATEGORY_PICKUP };
        }

        case ETeam::FriendlyPersistant:
        case ETeam::FriendlyFleeting:
        {
            return { CATEGORY_FRIENDLY, CATEGORY_NEUTRAL | CATEGORY_ENEMY };
        }

        case ETeam::Enemy:
        {
            return { CATEGORY_ENEMY, CATEGORY_NEUTRAL | CATEGORY_FRIENDLY };
        }

        case ETeam::Neutral:
        {
            return { CATEGORY_NEUTRAL, CATEGORY_PLAYER | CATEGORY_ENEMY | CATEGORY_FRIENDLY | CATEGORY_PROJECTILE };
        }

        case (ETeam::CoinDrop):
        {
            return { CATEGORY_PICKUP, CATEGORY_PLAYER };
        }

        default:
        {
            return { CATEGORY_NONE, CATEGORY_NONE };
        }
    }
}

class GameObject : public sf::Drawable
{
public:
    ~GameObject();

    void Destroy();
    bool IsDestroyed() const;

    void CreateBoxShapePhysicsBody(b2World * world, const sf::Vector2f & size, bool isDynamic, bool isSensor = true);
    bool CreateWedgeShapePhysicsBody(b2World * world, float arcAngleRad, float radius, int pointCount, bool isDynamic);
    void CreateCircleShapePhysicsBody(b2World * world, float radiusPixels, bool isSensor, bool isDynamic);
    void SetPhysicsBody(b2Body * pBody);

    void DestroyPhysicsBody(b2World * world);

    b2Body * GetPhysicsBody() const;

    void NotifyParentOfDeletion();

    // Add a single component of type T
    template <typename T>
    void AddComponent(std::shared_ptr<T> component)
    {
        static_assert(std::is_base_of<GameComponent, T>::value, "T must derive from GameComponent");
        mComponents[typeid(T)] = component; // Store shared ownership
    }

    // Get a single component of type T as a weak_ptr
    template <typename T>
    std::weak_ptr<T> GetComponent() const
    {
        auto it = mComponents.find(typeid(T));
        if (it != mComponents.end())
        {
            return std::weak_ptr<T>(std::static_pointer_cast<T>(it->second));
        }
        return std::weak_ptr<T>(); // Return an empty weak_ptr if the component doesn't exist
    }

    template <typename T>
    bool HasComponent() const
    {
        return mComponents.find(std::type_index(typeid(T))) != mComponents.end();
    }

    void Update(float deltaTime);

    ETeam GetTeam() const;
    void SetTeam(ETeam team);

    const sf::Vector2f & GetPosition() const;
    void SetPosition(const sf::Vector2f & position);

    float GetRotationDegrees() const;
    float GetRotationRadians() const;
    void SetRotation(float angle);
    sf::Vector2f GetSize() const;

    GameManager & GetGameManager() const;

    void AddChild(GameObject * pChild);
    void RemoveChild(GameObject * pChild);
    void GetChildren(std::vector<GameObject *> & childObjs);
    std::vector<BD::Handle> & GetChildrenHandles();

    GameObject & GetParent() const;
    BD::Handle GetParentHandle();
    void SetParent(BD::Handle parentHandle);

    std::vector<GameComponent *> GetAllComponents();

    BD::Handle GetHandle() const;

    void Activate();
    void Deactivate();
    void SetIsActive(bool active);
    bool IsActive();

    void DebugImGuiInfo();

    const float PIXELS_PER_METER = 100.f;

protected:
    GameObject(GameManager * pGameManager, ETeam team, BD::Handle handle, BD::Handle parentHandle = BD::Handle(0));

    void CleanUpChildren();

    virtual void draw(sf::RenderTarget & target, sf::RenderStates states) const override;

    std::unordered_map<std::type_index, std::shared_ptr<GameComponent>> mComponents;

private:
    void NotifyChildrenToDeactivate();

    bool mIsDestroyed; // Used to know when GameManager can remove from pool
    bool mActive; // Used to know when the GameObject is dying
    GameManager * mpGameManager;
    ETeam mTeam;
    std::vector<BD::Handle> mChildHandles;
    BD::Handle mHandle;
    BD::Handle mParentHandle;
    b2Body * mpPhysicsBody;

    friend class GameManager;
};

//------------------------------------------------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------------------------------------------------
