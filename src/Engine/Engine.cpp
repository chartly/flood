/************************************************************************
*
* vapor3D Engine � (2008-2010)
*
*	<http://www.vapor3d.org>
*
************************************************************************/

#include "Engine/API.h"
#include "Engine/Engine.h"

#include "Core/Memory.h"
#include "Resources/ResourceManager.h"
#include "Resources/ResourceLoader.h"
#include "Render/Texture.h"
#include "Render/Device.h"
#include "Render/Render.h"
#include "Input/InputManager.h"
#include "Audio/Device.h"
#include "Script/ScriptManager.h"
#include "Scene/Scene.h"
#include "Paging/PageManager.h"
#include "Physics/Physics.h"
#include "Network/Network.h"

NAMESPACE_ENGINE_BEGIN

//-----------------------------------//

static Engine* gs_engineInstance;
Engine* GetEngine() { return gs_engineInstance; }

Engine::Engine()
	: log(nullptr)
	, stream(nullptr)
	, fileSystem(nullptr)
	, taskPool(nullptr)
	, resourceManager(nullptr)
	, renderDevice(nullptr)
	, inputManager(nullptr)
	, audioDevice(nullptr)
	, physicsManager(nullptr)
	, scriptManager(nullptr)
{
	gs_engineInstance = this;
}

//-----------------------------------//

Engine::~Engine()
{
	scene.reset();

	for( size_t i = 0; i < subsystems.size(); i++ )
	{
		Subsystem* system = subsystems[i];
		delete system;
	}
	
	Deallocate(inputManager);
	Deallocate(physicsManager);
	Deallocate(scriptManager);
	Deallocate(renderDevice);
	Deallocate(resourceManager);
	Deallocate(audioDevice);

	InputDeinitialize();
	RenderDeinitialize();
	ResourcesDeinitialize();
	NetworkDeinitialize();

	TaskPoolDestroy(taskPool);
	StreamDestroy(stream);
	LogDestroy(log);
}

//-----------------------------------//

void Engine::addSubsystem( Subsystem* const subsystem )
{
	LogInfo( "Registering new engine subsystem" );
	
	subsystems.push_back( subsystem );
}

//-----------------------------------//

void Engine::init( bool createWindow )
{
	// Sets up the main logger.
	setupLogger();

	LogInfo( "Starting vapor3D" );

	// Creates the task system.
	taskPool = TaskPoolCreate( AllocatorGetHeap(), 2 );

	NetworkInitialize();
	ResourcesInitialize();
	InputInitialize();
	RenderInitialize();

	// Creates the resource manager.
	resourceManager = Allocate(ResourceManager, AllocatorGetThis());
	resourceManager->setTaskPool( taskPool );
	//resourceManager->setFileWatcher( fileSystem->getFileWatcher() );
	
	// Registers default resource loaders.
	resourceManager->setupResourceLoaders();

	// Creates the rendering and audio devices.
	setupDevices( createWindow );

	// Creates the initial scene.
	scene = Allocate(Scene, AllocatorGetThis());

#ifdef ENABLE_SCRIPTING_LUA
	// Creates the scripting manager.
	scriptManager = Allocate(ScriptManager, AllocatorGetThis());
#endif

#ifdef ENABLE_PHYSICS_BULLET
	// Creates the physics manager.
	physicsManager = Allocate(PhysicsManager, AllocatorGetThis());
#endif
}

//-----------------------------------//

void Engine::setScene(const ScenePtr& scene)
{
	this->scene = scene;
}

//-----------------------------------//

void Engine::setupLogger()
{
	stream = StreamCreateFromFile( AllocatorGetHeap(), "Log.html", StreamMode::Write);
	log = LogCreate( AllocatorGetHeap() );
}

//-----------------------------------//

void Engine::setupDevices( bool createWindow )
{
	renderDevice = new RenderDevice();

#ifdef ENABLE_AUDIO_OPENAL
	// Creates the audio device.
	audioDevice = new AudioDevice();
	audioDevice->createDevice("");
	audioDevice->createMainContext();
#endif
}

//-----------------------------------//

void Engine::setupInput()
{
	inputManager = new InputManager();
	inputManager->createDefaultDevices(); 
}

//-----------------------------------//

void Engine::update( float delta )
{
	for( size_t i = 0; i < subsystems.size(); i++ )
	{
		Subsystem* system = subsystems[i];
		system->update( delta );
	}

	resourceManager->update( delta );
	scene->update( delta );

#ifdef ENABLE_SCRIPTING_LUA
	scriptManager->update( delta );
#endif

#ifdef VAPOR_PHYSICS_BULLET
	physicsManager->update( delta );
#endif
}

//-----------------------------------//

void Engine::stepFrame()
{
	AllocatorReset( GetFrameAllocator() );
}

//-----------------------------------//

InputManager* Engine::getInputManager() const
{
#if 0
	Window* window = renderDevice->getWindow();

	if( !window )
		return nullptr;

	return window->getInputManager();
#endif

	return inputManager;
}

//-----------------------------------//

NAMESPACE_ENGINE_END