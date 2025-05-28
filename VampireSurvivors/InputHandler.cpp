#include "AstroidsPrivate.h"
#include "InputHandler.h"

namespace
{
    bool previousState[sf::Keyboard::KeyCount] = { false };
}

//------------------------------------------------------------------------------------------------------------------------

bool BD::IsKeyJustPressed(sf::Keyboard::Key key)
{
    bool isDown = sf::Keyboard::isKeyPressed(key);
    bool justPressed = isDown && !previousState[key];
    previousState[key] = isDown;
    return justPressed;
}

//------------------------------------------------------------------------------------------------------------------------

void BD::ResetKeyStates()
{
    for (int i = 0; i < sf::Keyboard::KeyCount; ++i)
        previousState[i] = false;
}

//------------------------------------------------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------------------------------------------------