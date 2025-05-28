#pragma once
#include "GameComponent.h"
class AbilitySelectionComponent : public GameComponent
{
public:
    AbilitySelectionComponent(GameObject * pOwner, GameManager & gameManager);
    ~AbilitySelectionComponent();

    virtual void Update(float deltaTime) override;
    virtual void draw(sf::RenderTarget & target, sf::RenderStates states) override;
    virtual void DebugImGuiComponentInfo() override;
    virtual std::string & GetClassName() override;

private:
    float mInteractionRange;
    bool mIsPlayerInRange;
    sf::Text mInteractionText;
    sf::Font mFont;
    std::string mName;
};

