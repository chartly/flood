/************************************************************************
*
* vapor3D Engine � (2008-2010)
*
*	<http://www.vapor3d.org>
*
************************************************************************/

#include "Engine/API.h"
#include "Engine.h"

#include "Core/Memory.h"
#include "Resources/ResourceManager.h"
#include "Resources/ResourceLoader.h"
#include "Render/Texture.h"
#include "Render/Device.h"
#include "Input/InputManager.h"
#include "Audio/Device.h"
#include "Script/ScriptManager.h"
#include "Scene/Scene.h"
#include "Paging/PageManager.h"
#include "Physics/Physics.h"

#include <ctime>

namespace vapor {

//-----------------------------------//

static Engine* engineInstance;
Engine* GetEngine() { return engineInstance; }

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
	engineInstance = this;
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
	
	delete inputManager;
	delete physicsManager;
	delete scriptManager;
	delete renderDevice;
	delete resourceManager;
	delete audioDevice;

	TaskPoolDestroy(taskPool);
	StreamDestroy(stream);
	LogDestroy(log);
}

//-----------------------------------//

void Engine::create(const std::string& app, const char** argv)
{
	this->app = app;
	this->argv = argv;
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

	// Creates the file system.
	//fileSystem = new FileSystem( app, argv ? argv[0] : nullptr );

	// Creates the task system.
	taskPool = TaskPoolCreate( AllocatorGetHeap(), 2 );

	// Creates the resource manager.
	resourceManager = new ResourceManager();
	//resourceManager->setFileWatcher( fileSystem->getFileWatcher() );
	resourceManager->setTaskPool( taskPool );
	
	// Registers default resource loaders.
	resourceManager->setupResourceLoaders();

	// Creates the rendering and audio devices.
	setupDevices( createWindow );

	// Creates the initial scene.
	scene.reset( new Scene() );

#ifdef ENABLE_SCRIPTING_LUA
	// Creates the scripting manager.
	scriptManager = new ScriptManager();
#endif

#ifdef VAPOR_PHYSICS_BULLET
	// Creates the physics manager.
	physicsManager = new PhysicsManager();
#endif
}

//-----------------------------------//

void Engine::setupLogger()
{
//	time_t rawtime;
//	time( &rawtime );
//
//	struct tm* timeinfo;
//
//#ifdef PLATFORM_WINDOWS
//	localtime_s( timeinfo, &rawtime );
//#else
//	timeinfo = localtime( &rawtime );
//#endif
//
//	char name[64];
//	
//	strftime(
//		name, ARRAY_SIZE(name),
//		"%Y_%m_%d-%H_%M_%S.html", timeinfo);

	stream = StreamCreateFromFile( AllocatorGetHeap(), "Log.html", StreamMode::Write);
	
	log = LogCreate( AllocatorGetHeap() );
	//log->add( new LogSinkHTML(*stream) );
}

//-----------------------------------//

void Engine::setupDevices( bool createWindow )
{
	renderDevice = new RenderDevice();

#ifdef VAPOR_AUDIO_OPENAL
	// Creates the audio device.
	audioDevice = new AudioDevice();
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

} // end namespace