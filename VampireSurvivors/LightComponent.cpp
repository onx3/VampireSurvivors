#include "AstroidsPrivate.h"
#include "LightComponent.h"

LightComponent::LightComponent(GameObject * pOwner, GameManager & gameManager, float radius, sf::Color color)
    : GameComponent(pOwner, gameManager)
    , mRadius(radius)
    , mColor(color)
    , mName("LightComponent")
{

}

//------------------------------------------------------------------------------------------------------------------------

LightComponent::~LightComponent()
{

}

//------------------------------------------------------------------------------------------------------------------------

void LightComponent::SetRadius(float radius)
{
    mRadius = radius;
}

//------------------------------------------------------------------------------------------------------------------------

void LightComponent::SetColor(const sf::Color & color)
{
    mColor = color;
}

//------------------------------------------------------------------------------------------------------------------------

float LightComponent::GetRadius() const
{
    return mRadius;
}

//------------------------------------------------------------------------------------------------------------------------

sf::Color LightComponent::GetColor() const
{
    return mColor;
}

//------------------------------------------------------------------------------------------------------------------------

const sf::Vector2f & LightComponent::GetPosition() const
{
    return GetGameObject().GetPosition();
}

//------------------------------------------------------------------------------------------------------------------------

void LightComponent::Update(float deltaTime)
{

}

//------------------------------------------------------------------------------------------------------------------------

void LightComponent::DebugImGuiComponentInfo()
{

}

//------------------------------------------------------------------------------------------------------------------------

std::string & LightComponent::GetClassName()
{
    return mName;
}

//------------------------------------------------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------------------------------------------------