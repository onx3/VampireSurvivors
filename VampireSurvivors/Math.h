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

    inline float GetMagnitudeSquared(const sf::Vector2f & vec1, const sf::Vector2f & vec2)
    {
        sf::Vector2f delta = vec2 - vec1;
        return delta.x * delta.x + delta.y * delta.y;
    }

    //------------------------------------------------------------------------------------------------------------------------

    inline float GetMagnitude(const sf::Vector2f & vec)
    {
        return std::sqrt(GetMagnitudeSquared(vec));
    }

    //------------------------------------------------------------------------------------------------------------------------

    inline float GetMagnitude(const sf::Vector2f & vec1, const sf::Vector2f & vec2)
    {
        return std::sqrt(GetMagnitudeSquared(vec1, vec2));
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

    //------------------------------------------------------------------------------------------------------------------------

    inline bool LineIntersectsAABB(const sf::Vector2f & a, const sf::Vector2f & b, const sf::FloatRect & box)
    {
        float tmin = 0.f;
        float tmax = 1.f;

        sf::Vector2f d = b - a;

        auto check = [&](float min, float max, float origin, float direction) -> bool {
            if (std::abs(direction) < 1e-6f)
            {
                return origin >= min && origin <= max;
            }
            float invD = 1.f / direction;
            float t1 = (min - origin) * invD;
            float t2 = (max - origin) * invD;
            if (t1 > t2) std::swap(t1, t2);
            tmin = std::max(tmin, t1);
            tmax = std::min(tmax, t2);
            return tmin <= tmax;
            };

        return check(box.left, box.left + box.width, a.x, d.x) &&
            check(box.top, box.top + box.height, a.y, d.y);
    }
}