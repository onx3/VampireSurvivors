#pragma once
#include <SFML/Window/Keyboard.hpp>

namespace BD
{
    bool IsKeyJustPressed(sf::Keyboard::Key key);
    void ResetKeyStates();
}