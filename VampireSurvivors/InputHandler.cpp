#include "AstroidsPrivate.h"
#include "InputHandler.h"

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