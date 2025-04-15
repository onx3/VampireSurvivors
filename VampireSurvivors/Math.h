#pragma once
#include <SFML/System/Vector2.hpp>
#include <cmath>

namespace BD
{
    inline float GetMagnitudeSquared(const sf::Vector2f & vec)
    {
        return vec.x * vec.x + vec.y * vec.y;
    }

    //------------------------------------------------------------------------------------------------------------------------

    inline float GetMagnitude(const sf::Vector2f & vec)
    {
        return std::sqrt(GetMagnitudeSquared(vec));
    }

    //------------------------------------------------------------------------------------------------------------------------

    inline float Dot(const sf::Vector2f & a, const sf::Vector2f & b)
    {
        return a.x * b.x + a.y * b.y;
    }

    //------------------------------------------------------------------------------------------------------------------------

    inline sf::Vector2f Normalize(const sf::Vector2f & vec)
    {
        float mag = GetMagnitude(vec);
        return (mag > 0.f) ? vec / mag : sf::Vector2f(0.f, 0.f);
    }
}