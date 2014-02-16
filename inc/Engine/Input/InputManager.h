/************************************************************************
*
* Flood Project © (2008-201x)
* Licensed under the simplified BSD license. All rights reserved.
*
************************************************************************/

#pragma once

#include "Engine/API.h"

NAMESPACE_ENGINE_BEGIN

//-----------------------------------//

class InputDevice;
struct InputEvent;

class Keyboard;
class Mouse;

/**
 * Manages a set of input devices.
 */

class API_INPUT InputManager
{
public:

    InputManager() = default;
    virtual ~InputManager() = default;

    InputManager(const InputManager&) = delete;
    InputManager& operator=(const InputManager&) = delete;

    /// Gets a keyboard device if it exists.
    virtual Keyboard* getKeyboard() = 0;
    
    /// Gets a mouse device if it exists.
    virtual Mouse* getMouse() = 0;
};

//-----------------------------------//

NAMESPACE_ENGINE_END