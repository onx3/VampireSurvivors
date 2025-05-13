#pragma once
#include "GameComponent.h"
class DoorComponent : public GameComponent
{
public:
    DoorComponent(GameObject * pOwner, GameManager & gameManager, int doorCost);

    virtual void Update(float deltaTime) override;
    virtual void DebugImGuiComponentInfo() override;
    virtual std::string & GetClassName() override;

    void Open();
    void Close();
    bool IsOpen();

private:
    bool mIsOpen;
    int mDoorCost;
    std::string mName;
};

