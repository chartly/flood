/************************************************************************
*
* Flood Project � (2008-201x)
* Licensed under the simplified BSD license. All rights reserved.
*
************************************************************************/

#pragma once

#include "Engine/API.h"
#include "Core/References.h"

NAMESPACE_ENGINE_BEGIN

//-----------------------------------//

class Stream;
class TaskPool;

class Subsystem;
class ResourceManager;
class RenderDevice;
class AudioDevice;
class PhysicsManager;
class PageManager;
class InputManager;
class ScriptManager;
class WindowManager;
class PlatformManager;

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
	
	Engine(PlatformManager* platform);
	~Engine();

	// Initialize the engine subsystems.
	void init();

	// Updates the main subsystems.
	void update();

	// Sets up the global engine logger.
	void setupLogger();

	// Gets the device.
	GETTER(TaskPool, TaskPool*, taskPool)

	// Gets/sets the platform manager.
	ACCESSOR(PlatformManager, PlatformManager*, platformManager)

	// Gets/sets the input manager.
	ACCESSOR(InputManager, InputManager*, inputManager)

	// Gets/sets the window manager.
	ACCESSOR(WindowManager, WindowManager*, windowManager)

	// Gets the render device.
	GETTER(RenderDevice, RenderDevice*, renderDevice)

	// Gets the audio device.
	GETTER(AudioDevice, AudioDevice*, audioDevice)

	// Gets the scripts manager.
	GETTER(ScriptManager, ScriptManager*, scriptManager)

	// Gets the resources manager.
	GETTER(ResourceManager, ResourceManager*, resourceManager)

	// Gets/sets the physics manager.
	ACCESSOR(PhysicsManager, PhysicsManager*, physicsManager)

	// Gets the main engine logger.
	GETTER(Logger, Log*, log)

	void stepFrame();

protected:

	// Manages background tasks.
	TaskPool* taskPool;

	// Default logger.
	Log* log;

	// Log stream.
	Stream* stream;

	// Platform manager.
	PlatformManager* platformManager;

	// Window manager.
	WindowManager* windowManager;

	// Input manager.
	InputManager* inputManager;

	// Rendering device.
	RenderDevice* renderDevice;

	// Audio device.
	AudioDevice* audioDevice;

	// Resource manager.
	ResourceManager* resourceManager;

	// Physics manager.
	PhysicsManager* physicsManager;

	// Script manager.
	ScriptManager* scriptManager;
};

// Gets the engine instance.
API_ENGINE Engine* GetEngine();

//-----------------------------------//

NAMESPACE_ENGINE_END