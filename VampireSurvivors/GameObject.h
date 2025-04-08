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
    NukeDrop,
    LifeDrop
};

class GameObject : public sf::Drawable
{
public:
    ~GameObject();

    void Destroy();
    bool IsDestroyed() const;

    void CreatePhysicsBody(b2World * world, const sf::Vector2f & size, bool isDynamic);
    bool CreateWedgeShapePhysicsBody(b2World * world, float arcAngleRad, float radius, int pointCount, bool isDynamic);
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

    sf::Vector2f GetPosition() const;
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

    GameObject * GetParent() const;
    BD::Handle GetParentHandle();
    void SetParent(BD::Handle parentHandle);

    std::vector<GameComponent *> GetAllComponents();

    BD::Handle GetHandle() const;

    void Activate();
    void Deactivate();
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
