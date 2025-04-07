#pragma once
#include "GameComponent.h"
class FollowComponent : public GameComponent
{
public:
    FollowComponent(GameObject * pOwner, GameManager & gameManager, BD::Handle followHandle, sf::Vector2f offset = { 0, 0 });
    ~FollowComponent();

    virtual void Update(float deltaTime) override;
    virtual std::string & GetClassName() override;

private:
    BD::Handle mFollowHandle;
    sf::Vector2f mOffset;
    std::string mName;
};

