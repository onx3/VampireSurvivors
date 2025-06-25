#pragma once
#include "GameComponent.h"
class LightComponent : public GameComponent
{
public:
    LightComponent(GameObject * pOwner, GameManager & gameManager, float radius, sf::Color color);
    ~LightComponent();

    virtual void Update(float deltaTime) override;
    virtual void DebugImGuiComponentInfo() override;
    virtual const std::string & GetClassName() override;

    void SetRadius(float radius);
    void SetColor(const sf::Color & color);

    float GetRadius() const;
    sf::Color GetColor() const;
    const sf::Vector2f & GetPosition() const;

private:
    float mRadius;
    sf::Color mColor;

    std::string mName;
};

