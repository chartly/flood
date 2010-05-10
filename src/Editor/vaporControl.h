/************************************************************************
*
* vapor3D Editor � (2008-2010)
*
*	<http://www.vapor3d.org>
*
************************************************************************/

#pragma once

#include <wx/glcanvas.h>
#include "vaporInputManager.h"

namespace vapor { namespace editor {

class vaporWindow;

//-----------------------------------//

/**
 * Let's link vapor with wxWidgets. There are some different approaches
 * that can be used to do this, some more flexible than others. We could
 * create a custom wxWidgets window impementation in vapor, for example,
 * but this class approaches the problem by getting just an OS-specific
 * window handle, and letting vaporEngine do all the dirty work of setting
 * up the rendering context and implementing the window class.
 * UPDATE: SDL didn't work with external window handles, I had to patch
 * it myself. I ended up creating what I said above, a specific window
 * implementation for vapor based on wxGLCanvas. Now we use SFML instead
 * of SDL, and it has support for external window handles, so it might
 * work well with the window handle approach.
 */

class vaporControl : public wxGLCanvas 
{
public:

	vaporControl( 	wxWindow* parent, wxWindowID id = wxID_ANY,
					const int* attribList = nullptr,
					const wxPoint& pos	= wxDefaultPosition,
					const wxSize& size = wxDefaultSize,
					long style = 0 | wxFULL_REPAINT_ON_RESIZE | wxSTATIC_BORDER,
					const wxString&	name = "vaporGLCanvas",
					const wxPalette& palette = wxNullPalette ); 	

	// Add your frame updating code here.
	fd::delegate<void(double)> onUpdate;

	// Add your frame rendering code here.
	fd::delegate<void()> onRender;

	// Flag this control to be redrawn.
	void flagRedraw();

	// Starts firing the update and render frame timers.
	void startFrameLoop();

	// Starts firing the update and render frame timers.
	void stopFrameLoop();

	// Gets the associated window.
	IMPLEMENT_GETTER(RenderWindow, vaporWindow*, window)

	// Gets the associated input manager.
	IMPLEMENT_GETTER(InputManager, vaporInputManager*, inputManager)

protected:

	// Handles the update and redraw logic.
	void doUpdate(wxTimerEvent&);
	void doRender(wxTimerEvent&);

	// Window events.
	void OnIdle(wxIdleEvent& event);
	void OnPaint(wxPaintEvent& event);
	void OnSize(wxSizeEvent& event);
	void OnFocusSet(wxFocusEvent& event);
	void OnFocusKill(wxFocusEvent& event);

	// Input events.
	void OnKeyDown(wxKeyEvent& event);
	void OnKeyUp(wxKeyEvent& event);
	void OnMouseEvent(wxMouseEvent& event);
	void OnMouseCaptureLost(wxMouseCaptureLostEvent& event);

	// Frame timers.
	wxTimer frameUpdateTimer;
	wxTimer frameRenderTimer;

	// Holds an instance to the vaporEngine.
	Engine* engine;

	// Window associated with this control.
	vaporWindow* window;

	// Holds an instance of the input manager.
	vaporInputManager* inputManager;

	// Tracks if the control needs to be redrawn.
	bool needsRedraw;

	DECLARE_EVENT_TABLE()
};

//-----------------------------------//

} } // end namespaces