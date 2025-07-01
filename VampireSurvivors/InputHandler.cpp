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
// InputHandler
//------------------------------------------------------------------------------------------------------------------------

InputHandler::InputHandler()
{
    mPreviousState.fill(false);
    mCurrentState.fill(false);
}

//------------------------------------------------------------------------------------------------------------------------

void InputHandler::Update()
{
    mPreviousState = mCurrentState;
    for (int ii = 0; ii < sf::Keyboard::KeyCount; ++ii)
    {
        mCurrentState[ii] = sf::Keyboard::isKeyPressed(sf::Keyboard::Key(ii));
    }
}

//------------------------------------------------------------------------------------------------------------------------

bool InputHandler::IsKeyJustPressed(sf::Keyboard::Key key) const
{
    return !mPreviousState[key] && mCurrentState[key];
}

//------------------------------------------------------------------------------------------------------------------------

bool InputHandler::IsKeyHeld(sf::Keyboard::Key key) const
{
    return mCurrentState[key];
}

//------------------------------------------------------------------------------------------------------------------------

bool InputHandler::IsKeyJustReleased(sf::Keyboard::Key key) const
{
    return mPreviousState[key] && !mCurrentState[key];
}

//------------------------------------------------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------------------------------------------------