#pragma once
#include "GameComponent.h"
#include <box2d/box2d.h>

class CollisionComponent : public GameComponent
{
public:
    CollisionComponent(GameObject * pOwner, GameManager & gameManager, b2World * pWorld, b2Body * pBody, sf::Vector2f size, bool isDynamic);
    ~CollisionComponent();

    virtual void Update(float deltaTime) override;
    virtual void DebugImGuiComponentInfo() override;
    virtual std::string & GetClassName() override;

private:
    b2Body * mpBody;
    b2World * mpWorld;
    sf::Vector2f mSize;
    std::string mName;
};

//------------------------------------------------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------------------------------------------------