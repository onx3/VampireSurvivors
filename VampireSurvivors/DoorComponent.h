#pragma once
#include "GameComponent.h"
class DoorComponent : public GameComponent
{
public:
    DoorComponent(GameObject * pOwner, GameManager & gameManager, int doorCost);

    virtual void Update(float deltaTime) override;
    virtual void draw(sf::RenderTarget & target, sf::RenderStates states) override;
    virtual void DebugImGuiComponentInfo() override;
    virtual const std::string & GetClassName() override;

    void Open();
    void Close();
    bool IsOpen();

private:
    bool mIsOpen;
    int mDoorCost;
    float mInteractionRange;
    bool mIsPlayerInRange;
    sf::Text mInteractionText;
    sf::Font mFont;
    std::string mName;
};

