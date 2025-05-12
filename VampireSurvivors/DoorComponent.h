#pragma once
#include "GameComponent.h"
class DoorComponent : public GameComponent
{
public:
    DoorComponent(GameObject * pOwner, GameManager & gameManager);

    virtual void Update(float deltaTime) override;
    virtual void DebugImGuiComponentInfo() override;
    virtual std::string & GetClassName() override;

    void Open();
    void Close();
    bool IsOpen();

private:
    bool mIsOpen;
    std::string mName;
};

