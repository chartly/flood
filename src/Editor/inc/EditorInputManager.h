/************************************************************************
*
* vapor3D Editor � (2008-2010)
*
*	<http://www.vapor3d.org>
*
************************************************************************/

#pragma once

namespace vapor { namespace editor {

//-----------------------------------//

/**
 * Forwards all the input received by wxWidgets to the engine.
 */

class EditorInputManager : public InputManager
{
public:

	EditorInputManager();

	// Processes a wxWidgets keyboard input event.
	void processKeyEvent( const wxKeyEvent&, bool keyDown );

	// Processes a wxWidgets mouse input event.
	void processMouseEvent( const wxMouseEvent& );

	// Custom processing of input events.
	void doKeyEvent( const wxKeyEvent&, bool keyDown );
	void doMouseEvent( const wxMouseEvent& );

private:

	// Converts from wxWidgets events to vapor3D events.
	static Keys::Enum convertKeyEnum( int keyCode );
};

//-----------------------------------//

} } // end namespaces