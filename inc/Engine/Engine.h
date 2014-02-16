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

/**
 * Main engine class. This is a utility class that instantiates all the
 * other engine classes and provides suitable methods to get/set them.
 * It is also responsible to set the default logger for all of the engine
 * logging needs. If you need extra flexibility in your app, you can create
 * a class like this one that instantiates everything and sets it all up.
 */

class API_ENGINE Engine
{
public:
	
	Engine();
	~Engine();

	// Initialize the engine subsystems.
	void init();

	// Updates the main subsystems.
	void update();
};

// Gets the engine instance.
API_ENGINE Engine* fldEngine();

//-----------------------------------//

NAMESPACE_ENGINE_END