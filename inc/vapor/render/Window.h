/************************************************************************
*
* vaporEngine (2008-2009)
*
*	<http://www.portugal-a-programar.org>
*
************************************************************************/

#pragma once

#include "vapor/Platform.h"
#include "vapor/render/Target.h"

#include "vapor/input/InputManager.h"

namespace vapor {
	namespace render {

//-----------------------------------//

/**
 * Represents the information that the user wants to pass to the engine
 * related to the creation of a new rendering window, like for example
 * bits-per-pixel, the size of the the window, if it is fullscreen, and 
 * in the future, anti-aliasing settings, and custom window handle hooks.
 */

class WindowSettings : public Settings
{
public:

	WindowSettings( const int width = 640, const int height = 480,
		const bool fullscreen = false, 
		const std::string title = "Untitled",
		const int bpp = 32, 
		const int depthbits = 24, 
		const int stencilbits = 8,
		const int aalevel = 0, 
		void* customHandle = nullptr );

	// Gets the window title
	const std::string& getTitle() const { return title; }

	// Gets the custom handle of the window 
	void* getCustomHandle() const { return customHandle; }
	
	// Is this window fullscreen?
	const bool isFullscreen() const { return fullscreen; }
	
	// Gets the bits-per-pixel of the window
	const int getBpp() const { return bpp; }

	// Gets the size of the window's depth buffer 
	const int getDepthBits() const { return depthbits; }
	
	// Gets the size of the window's stencil buffer 
	const int getStencilBits() const { return stencilbits; }
	
	// Gets the antialiasing level of the window
	const int getAALevel() const { return aalevel; }

	// Sets the window title
	void setTitle(const std::string& str) { title = str; }
	
	// Sets the size of the window's depth buffer 
	void setDepthBits(int db) { depthbits = db; }
	
	// Sets the size of the window's stencil buffer 
	void setStencilBits(int sb) { stencilbits = sb; }
	
	// Sets the antialiasing level of the window
	void setAALevel(int aal) { aalevel = aal; }

protected:

	int bpp;
	int depthbits;
	int stencilbits;
	int aalevel;
	
	bool fullscreen;
	
	void* customHandle;
	
	std::string title;
};

//-----------------------------------//

/**
 * Represents an region on the screen that the renderer can send
 * the final representation of the scene, the rendered output image.
 * In most platforms (PCs) this will be just a normal window on the
 * desktop, but on some platforms (consoles, for instance) this might 
 * be slighty different, so some methods might not make much sense. 
 */

class Window : public RenderTarget
{
public:

	Window (const WindowSettings& settings);
	virtual ~Window ();

	// Updates the window content.
	virtual void update() = 0;

	// Shows/hides the window.
	virtual void show( bool hide = false ) = 0;

	// Handles the event loop of the window.
	virtual bool pumpEvents() = 0;

	// Sets the title of the window.
	virtual void setTitle (const std::string& title) = 0;

	// Sets the visibility of the mouse cursor.
	virtual void setCursor ( bool state ) = 0;

	// Gets the window settings.
	virtual const Settings& getSettings();

	// Gets the input manager.
	virtual input::InputManager& getInputManager() = 0;

	// Create a new render window.
	static Window& createWindow( 
		const WindowSettings& settings = WindowSettings() );

	// Event fired when the window is closed.
	fd::delegate< void( void ) > onWindowClose;

protected:

	// Handles the resize event.
	void handleWindowResize();

	// Handles the resize event.
	void handleWindowClose();

	// Holds the window settings
	WindowSettings settings;
};

//-----------------------------------//

} } // end namespaces