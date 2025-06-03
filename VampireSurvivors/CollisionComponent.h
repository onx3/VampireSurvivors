#pragma once
#include "GameComponent.h"
#include <box2d/box2d.h>

class CollisionComponent : public GameComponent
{
public:
    CollisionComponent(GameObject * pOwner, GameManager & gameManager, b2World * pWorld, b2Body * pBody, sf::Vector2f size, bool isDynamic , bool isActive = true);
    ~CollisionComponent();

    virtual void Update(float deltaTime) override;
    virtual void DebugImGuiComponentInfo() override;
    virtual std::string & GetClassName() override;

    void SetActive(bool active);

    bool IntersectsLine(const sf::Vector2f & a, const sf::Vector2f & b) const;
    sf::FloatRect GetWorldAABB() const;

private:
    b2Body * mpBody;
    b2World * mpWorld;
    sf::Vector2f mSize;
    bool mIsActive;
    std::string mName;
};

//------------------------------------------------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------------------------------------------------