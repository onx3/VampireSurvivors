#pragma once
#include <SFML/Window/Keyboard.hpp>

class InputHandler
{
public:
    InputHandler();

    void Update();

    bool IsKeyJustPressed(sf::Keyboard::Key key) const;
    bool IsKeyHeld(sf::Keyboard::Key key) const;
    bool IsKeyJustReleased(sf::Keyboard::Key key) const;

private:
    std::array<bool, sf::Keyboard::KeyCount> mPreviousState{};
    std::array<bool, sf::Keyboard::KeyCount> mCurrentState{};
};