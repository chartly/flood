/************************************************************************
*
* Flood Project © (2008-201x)
* Licensed under the simplified BSD license. All rights reserved.
*
************************************************************************/

#include "Engine/API.h"
#include "Engine/Engine.h"
#include "Engine/Subsystem.h"

#include "Core/Memory.h"
#include "Core/Network/Network.h"
#include "Resources/ResourceManager.h"
#include "Resources/ResourceLoader.h"
#include "Graphics/Texture.h"
#include "Graphics/RenderDevice.h"
#include "Engine/PlatformManager.h"
#include "Engine/Audio/Device.h"
#include "Engine/Input/InputManager.h"
#include "Engine/Paging/PageManager.h"
#include "Engine/Physics/Physics.h"
#include "Engine/Window/WindowManager.h"
#include "Script/ScriptManager.h"

#ifdef API_ENGINE_DLL_EXPORT
FL_INSTANTIATE_TEMPLATES()
#endif

NAMESPACE_ENGINE_BEGIN

//-----------------------------------//

static Engine* gs_engineInstance;
Engine* fldEngine() { return gs_engineInstance; }

Engine::Engine(PlatformManager* platformManager)
    : platformManager(platformManager)
    , resourceManager(nullptr)
    , renderDevice(nullptr)
    , audioDevice(nullptr)
    , physicsManager(nullptr)
    , scriptManager(nullptr)
{
    assert(gs_engineInstance == nullptr);
    gs_engineInstance = this;

    assert(platformManager && "Expected a valid platform manager");
}

//-----------------------------------//

Engine::~Engine()
{
    Deallocate(physicsManager);
    Deallocate(scriptManager);
    Deallocate(renderDevice);

#ifdef ENABLE_AUDIO_OPENAL
    Deallocate(audioDevice);
#endif

    Deallocate(resourceManager);

    InputDeinitialize();
    GraphicsDeinitialize();
    ResourcesDeinitialize();

#ifdef ENABLE_NETWORK_ENET
    NetworkDeinitialize();
#endif

    Core::shutdown();
}

//-----------------------------------//

void Engine::init()
{
    Core::init();

    // Initialize the platform-specific subsystems.
    platformManager->init();

#ifdef ENABLE_NETWORK_ENET
    NetworkInitialize();
#endif

    ResourcesInitialize();
    GraphicsInitialize();
    InputInitialize();

    // Creates the resource manager.
    resourceManager = AllocateThis(ResourceManager);
    resourceManager->setTaskPool( fldCore()->taskPool );
    
    // Registers default resource loaders.
    resourceManager->setupResourceLoaders( ResourceLoaderGetType() );

    // Creates the rendering device.
    renderDevice = AllocateThis(RenderDevice);

#ifdef ENABLE_AUDIO_OPENAL
    // Creates the audio device.
    audioDevice = AudioCreateDevice("");

    if( audioDevice )
        audioDevice->createMainContext();
#endif

#ifdef ENABLE_SCRIPTING_LUA
    // Creates the scripting manager.
    scriptManager = AllocateThis(ScriptManager);
#endif
}

//-----------------------------------//

void Engine::update()
{
    resourceManager->update();
    platformManager->update();

#ifdef ENABLE_SCRIPTING_LUA
    scriptManager->update();
#endif

    AllocatorReset(fldCore()->stack);
}

//-----------------------------------//

NAMESPACE_ENGINE_END