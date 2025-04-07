#include "AstroidsPrivate.h"
#include "GameObject.h"
#include <cassert>
#include <imgui.h>
#include "SpriteComponent.h"
#include "BDConfig.h"
#include "GameComponent.h"
#include "PlayerManager.h"

//------------------------------------------------------------------------------------------------------------------------

GameObject::GameObject(GameManager * pGameManager, ETeam team, BD::Handle handle, BD::Handle parentHandle)
    : mpGameManager(pGameManager)
    , mIsDestroyed(false)
    , mActive(true)
    , mTeam(team)
    , mChildHandles()
    , mHandle(handle)
    , mParentHandle(parentHandle)
{
}

//------------------------------------------------------------------------------------------------------------------------

GameObject::~GameObject()
{
    CleanUpChildren();
}

//------------------------------------------------------------------------------------------------------------------------

void GameObject::CleanUpChildren()
{
    for (auto childHandle : mChildHandles)
    {
        auto * pChild = GetGameManager().GetGameObject(childHandle);
        if (pChild)
        {
            pChild->CleanUpChildren();
            GetGameManager().RemoveGameObject(childHandle);
        }
    }
    mChildHandles.clear();
}

//------------------------------------------------------------------------------------------------------------------------

void GameObject::Destroy()
{
    mIsDestroyed = true;
    for (auto childHandle : mChildHandles)
    {
        auto * pChild = GetGameManager().GetGameObject(childHandle);
        if (pChild)
        {
            pChild->Destroy();
        }
    }
}

//------------------------------------------------------------------------------------------------------------------------

bool GameObject::IsDestroyed() const
{
    return mIsDestroyed;
}

//------------------------------------------------------------------------------------------------------------------------

void GameObject::CreatePhysicsBody(b2World * world, const sf::Vector2f & size, bool isDynamic)
{
    // Define the body
    b2BodyDef bodyDef;
    bodyDef.type = isDynamic ? b2_dynamicBody : b2_staticBody;
    bodyDef.position.Set(GetPosition().x / PIXELS_PER_METER, GetPosition().y / PIXELS_PER_METER);
    bodyDef.bullet = true; // More acurate collision checks
    bodyDef.awake = true;

    // Create the body in the Box2D world
    mpPhysicsBody = world->CreateBody(&bodyDef);

    // Define the shape
    b2PolygonShape boxShape;
    boxShape.SetAsBox((size.x / 2.0f) / PIXELS_PER_METER, (size.y / 2.0f) / PIXELS_PER_METER);

    // Define the fixture
    b2FixtureDef fixtureDef;
    fixtureDef.shape = &boxShape;
    fixtureDef.density = isDynamic ? 1.0f : 0.0f;
    fixtureDef.friction = 0.3f;

    // Attach the fixture to the body
    mpPhysicsBody->CreateFixture(&fixtureDef);

    // Set user data for later use
    mpPhysicsBody->GetUserData().pointer = reinterpret_cast<uintptr_t>(this);
}

//------------------------------------------------------------------------------------------------------------------------

void GameObject::DestroyPhysicsBody(b2World * world)
{
    if (mpPhysicsBody)
    {
        world->DestroyBody(mpPhysicsBody);
        mpPhysicsBody = nullptr;
    }
}

//------------------------------------------------------------------------------------------------------------------------

b2Body * GameObject::GetPhysicsBody() const
{
    return mpPhysicsBody;
}

//------------------------------------------------------------------------------------------------------------------------

void GameObject::NotifyParentOfDeletion()
{
    auto * pParent = GetGameManager().GetGameObject(mParentHandle);
    if (pParent)
    {
        std::vector<GameObject *> childObjs;
        pParent->GetChildren(childObjs);
        auto it = std::find(childObjs.begin(), childObjs.end(), this);
        if (it != childObjs.end())
        {
            childObjs.erase(it);
        }
    }
}


//------------------------------------------------------------------------------------------------------------------------

void GameObject::Update(float deltaTime)
{
    if (!mIsDestroyed)
    {
        // Update components
        for (auto & component : mComponents)
        {
            component.second->Update(deltaTime);
        }

        // Update child objects
        for (auto childHandle : mChildHandles)
        {
            auto * pChild = GetGameManager().GetGameObject(childHandle);
            if (pChild && !pChild->IsDestroyed())
            {
                pChild->Update(deltaTime);
            }
        }
    }
}

//------------------------------------------------------------------------------------------------------------------------

ETeam GameObject::GetTeam() const
{
    return mTeam;
}

//------------------------------------------------------------------------------------------------------------------------

void GameObject::SetTeam(ETeam team)
{
    mTeam = team;
}

//------------------------------------------------------------------------------------------------------------------------

sf::Vector2f GameObject::GetPosition() const
{
    auto pGameObjectSprite = GetComponent<SpriteComponent>().lock();
    if (pGameObjectSprite)
    {
        return pGameObjectSprite->GetPosition();
    }
    return sf::Vector2f();
}

//------------------------------------------------------------------------------------------------------------------------

void GameObject::SetPosition(const sf::Vector2f & position)
{
    auto pGameObjectSprite = GetComponent<SpriteComponent>().lock();
    if (pGameObjectSprite)
    {
        return pGameObjectSprite->SetPosition(position);
    }
}

//------------------------------------------------------------------------------------------------------------------------

float GameObject::GetRotationDegrees() const
{
    auto pGameObjectSprite = GetComponent<SpriteComponent>().lock();
    if (pGameObjectSprite)
    {
        return pGameObjectSprite->GetRotation();
    }
    return 0.0f;
}

//------------------------------------------------------------------------------------------------------------------------

float GameObject::GetRotationRadians() const
{
    auto pGameObjectSprite = GetComponent<SpriteComponent>().lock();
    if (pGameObjectSprite)
    {
        return pGameObjectSprite->GetRotation() * (3.14159265f / 180.f);
    }
    return 0.0f;
}

//------------------------------------------------------------------------------------------------------------------------

void GameObject::SetRotation(float angle)
{
    auto pGameObjectSprite = GetComponent<SpriteComponent>().lock();
    if (pGameObjectSprite)
    {
        return pGameObjectSprite->SetRotation(angle);
    }
}

//------------------------------------------------------------------------------------------------------------------------

sf::Vector2f GameObject::GetSize() const
{
    auto pGameObjectSprite = GetComponent<SpriteComponent>().lock();
    if (pGameObjectSprite)
    {
        return pGameObjectSprite->GetSprite().getGlobalBounds().getSize();
    }
    return sf::Vector2f();
}

//------------------------------------------------------------------------------------------------------------------------

GameManager & GameObject::GetGameManager() const
{
    assert(mpGameManager && "mpGameManager is nullptr!");
    return *mpGameManager;
}

//------------------------------------------------------------------------------------------------------------------------

void GameObject::AddChild(GameObject * pChild)
{
    if (pChild)
    {
        mChildHandles.push_back(pChild->GetHandle());
        pChild->SetParent(GetHandle());
    }
}

//------------------------------------------------------------------------------------------------------------------------

void GameObject::RemoveChild(GameObject * pChild)
{
    auto it = std::find(mChildHandles.begin(), mChildHandles.end(), pChild->GetHandle());
    if (it != mChildHandles.end())
    {
        // Get the actual GameObject from the pool
        GameObject * childObject = GetGameManager().GetGameObject(*it);
        if (childObject)
        {
            childObject->SetParent(BD::Handle(0));
        }

        mChildHandles.erase(it);
    }
}

//------------------------------------------------------------------------------------------------------------------------

void GameObject::GetChildren(std::vector<GameObject *> & childObjs)
{
    childObjs.reserve(mChildHandles.size());
    for (auto childHandle : mChildHandles)
    {
        childObjs.push_back(GetGameManager().GetGameObject(childHandle));
    }
}

//------------------------------------------------------------------------------------------------------------------------

std::vector<BD::Handle> & GameObject::GetChildrenHandles()
{
    return mChildHandles;
}

//------------------------------------------------------------------------------------------------------------------------

GameObject * GameObject::GetParent() const
{
    return GetGameManager().GetGameObject(mParentHandle);
}

//------------------------------------------------------------------------------------------------------------------------

BD::Handle GameObject::GetParentHandle()
{
    return mParentHandle;
}

//------------------------------------------------------------------------------------------------------------------------

void GameObject::SetParent(BD::Handle parentHandle)
{
    mParentHandle = parentHandle;
}

//------------------------------------------------------------------------------------------------------------------------

std::vector<GameComponent *> GameObject::GetAllComponents()
{
    std::vector<GameComponent *> components;

    if (!this)
    {
        return components;
    }

    if (mComponents.empty() && mComponents.size() > 0)
    {
        return components;
    }

    for (const auto & [type, component] : mComponents)
    {
        if (component)
        {
            components.push_back(component.get());
        }
    }

    return components;
}

//------------------------------------------------------------------------------------------------------------------------

BD::Handle GameObject::GetHandle() const
{
    return mHandle;
}

//------------------------------------------------------------------------------------------------------------------------

void GameObject::Activate()
{
    mActive = true;
}

//------------------------------------------------------------------------------------------------------------------------

void GameObject::Deactivate()
{
    NotifyChildrenToDeactivate();
    mActive = false;
}

//------------------------------------------------------------------------------------------------------------------------

void GameObject::NotifyChildrenToDeactivate()
{
    for (auto childHandle : mChildHandles)
    {
        auto pChildObj = GetGameManager().GetGameObject(childHandle);
        pChildObj->Deactivate();
    }
}

//------------------------------------------------------------------------------------------------------------------------

bool GameObject::IsActive()
{
    return mActive;
}

//------------------------------------------------------------------------------------------------------------------------

void GameObject::DebugImGuiInfo()
{
    GameManager & gameManager = GetGameManager();
#if IMGUI_ENABLED()
    for (auto & component : mComponents)
    {
        auto playerHandle = gameManager.GetManager<PlayerManager>()->GetPlayers()[0];
        auto * pPlayer = gameManager.GetGameObject(playerHandle);
        if (this == pPlayer)
        {
            std::vector<GameObject *> childObjs;
            pPlayer->GetChildren(childObjs);
            ImGui::Text("Children count: %zu", childObjs.size());
        }
        // Update each component
        if (ImGui::CollapsingHeader(component.first.name()))
        {
            component.second->DebugImGuiComponentInfo();
        }
    }
#endif
}

//------------------------------------------------------------------------------------------------------------------------

void GameObject::draw(sf::RenderTarget & target, sf::RenderStates states) const
{
    for (auto & pComponent : mComponents)
    {
        pComponent.second->draw(target, states);
    }

    for (auto childHandle : mChildHandles)
    {
        auto * pChild = GetGameManager().GetGameObject(childHandle);
        if (pChild)
        {
            target.draw(*pChild, states);
        }
    }
}

//------------------------------------------------------------------------------------------------------------------------
//EOF
//------------------------------------------------------------------------------------------------------------------------