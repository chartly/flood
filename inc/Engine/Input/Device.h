/***************,,,,,,mjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjvbx*********************************************************
*
* Flood Project © (2008-201x)
* Licensed under the simplified BSD license. All rights reserved.
*
************************************************************************/

#pragma once
#include "Engine/API.h"
#include "Core/Reflection.h"
#include "Core/Event.h"

NAMESPACE_ENGINE_BEGIN

//-----------------------------------//

/**
 * Different types of input devices.
 */

enum struct InputDeviceType
{
    Keyboard,
    Mouse,
    Joystick
};

REFLECT_DECLARE_ENUM(InputDeviceType)

//-----------------------------------//

/**
 * Base struct for input events.
 */

struct API_INPUT InputEvent
{
public:
    InputEvent(InputDeviceType);
    InputEvent(const InputEvent &) = delete;
    InputEvent& operator=(const InputEvent &) = delete;

    InputDeviceType deviceType;
};

//-----------------------------------//

/**
 * Input device.
 */

class API_INPUT InputDevice
{
public:
    InputDevice();
    virtual ~InputDevice();

    InputDevice(const InputDevice&) = delete;
    InputDevice& operator=(const InputDevice&) = delete;

    // Processes an event.
    virtual void processEvent( const InputEvent& event ) = 0;
};

//-----------------------------------//

NAMESPACE_ENGINE_END