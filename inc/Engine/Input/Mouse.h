/************************************************************************
*
* Flood Project © (2008-201x)
* Licensed under the simplified BSD license. All rights reserved.
*
************************************************************************/

#pragma once

#include "Engine/API.h"
#include "Engine/Input/Device.h"
#include "Engine/Input/MouseEvents.h"
#include "Core/Event.h"

NAMESPACE_ENGINE_BEGIN

//-----------------------------------//

/**
 * Holds the mouse state.
 */

struct API_ENGINE MouseInfo
{
    MouseInfo();

    // Position state.
    short x;
    short y;
    bool insideWindow;
    
    // Buttons state.
    bool leftButton;
    bool rightButton;
    bool middleButton;
    bool aux1Button;
    bool aux2Button;
};

//-----------------------------------//

class API_ENGINE Mouse : public InputDevice
{
public:

    // Processes a mouse input event.
    FLD_IGNORE virtual void processEvent(const InputEvent& event) override;

    // Returns whether a given mouse button is pressed.
    bool isButtonPressed( MouseButton button ) const;

    // Mouse move event.
    Event1< const MouseMoveEvent& > onMouseMove;

    // Mouse drag event.
    Event1< const MouseDragEvent& > onMouseDrag;

    // Mouse button press event.
    Event1< const MouseButtonEvent& > onMouseButtonPress;

    // Mouse button release event.
    Event1< const MouseButtonEvent& > onMouseButtonRelease;

    // Mouse wheel move event.
    Event1< const MouseWheelEvent& > onMouseWheelMove;

    // Mouse enter window event.
    Event0<> onMouseEnter;

    // Mouse exit window event.
    Event0<> onMouseExit;

    // Holds the current state of the mouse.
    MouseInfo mouseInfo;

private:

    // Occurs when a mouse button is pressed.
    void mouseButtonPressed(const MouseButtonEvent& event);

    // Occurs when a mouse button is released.
    void mouseButtonReleased(const MouseButtonEvent& event);

    // Occurs when the mouse is moved.
    void mouseMoved(const MouseMoveEvent& event);

    // Occurs when the mouse is dragged.
    void mouseDragged(const MouseDragEvent& event);

    // Occurs when the mouse cursor enters the window.
    void mouseEnter();

    // Occurs when the mouse cursor exits the window.
    void mouseExit();

    // Occurs when the mouse wheel is scrolled.
    void mouseWheelMove(const MouseWheelEvent& event);

    // Sets the button state in the mouse info.
    void setMouseButton( MouseButton button, bool state );
};

//-----------------------------------//

NAMESPACE_ENGINE_END
