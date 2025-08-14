#pragma once
class Camera2D
{
public:
    Camera2D(sf::RenderWindow * pWindow);
    virtual ~Camera2D();

    void Move(float dx, float dy);

    void SetPosition(float x, float y);
    sf::Vector2f GetPosition() const;

    void Zoom(float factor);

    sf::Vector2f ScreenToWorld(const sf::Vector2i & screenPos) const;
    sf::Vector2i WorldToScreen(const sf::Vector2f & worldPos) const;

private:
    sf::RenderWindow * mpWindow;
    sf::View mView;
};

