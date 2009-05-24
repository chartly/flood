/************************************************************************
*
* vaporEngine by triton � (2008-2009)
*
*	<http://www.portugal-a-programar.org>
*
************************************************************************/

#pragma once

#include "vapor/CompileOptions.h"

#ifdef VAPOR_WINDOWING_SDL

#include <SDL.h>

#include "vapor/Platform.h"
#include "vapor/render/Window.h"

namespace vapor {
	namespace render {

class SDLWindow : public Window
{
public:

	SDLWindow(Settings& settings);
	virtual ~SDLWindow();

	// Swaps the buffers (updates the display)
	void update ();

	// Handle the window message events
	bool pump();

	// Sets the title of the window
	void setTitle(const string& title) const;

	// Sets the cursor visibility
	void setCursor(bool state) const;

private:

	// Display surface
	SDL_Surface	*display;			

	// Initializes SDL
	bool init();

	// Opens a new window
	bool open();
};

} } // end namespaces

#endif