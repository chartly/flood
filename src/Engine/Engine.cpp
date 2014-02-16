/************************************************************************
*
* Flood Project © (2008-201x)
* Licensed under the simplified BSD license. All rights reserved.
*
************************************************************************/

#include "Engine/API.h"
#include "Engine/Engine.h"

#include "Engine/Input/InputManager.h"
#include "Resources/ResourceManager.h"
#include "Resources/ResourceLoader.h"
#include "Core/FileWatcher.h"
#include "Core/Memory.h"

#ifdef API_ENGINE_DLL_EXPORT
FL_INSTANTIATE_TEMPLATES()
#endif

NAMESPACE_ENGINE_BEGIN

//-----------------------------------//

static Engine* gs_engineInstance;
Engine* fldEngine() { return gs_engineInstance; }

Engine::Engine()
{
    assert(gs_engineInstance == nullptr);
    gs_engineInstance = this;
}

//-----------------------------------//

Engine::~Engine()
{
    Deallocate(fldCore()->resourceManager);

    ResourcesDeinitialize();

    Core::shutdown();
}

//-----------------------------------//

void Engine::init()
{
    Core::init();

    ResourcesInitialize();

    // Creates the resource manager.
    fldCore()->resourceManager = AllocateThis(ResourceManager);
    auto res = fldCore()->resourceManager;
    
    // Registers default resource loaders.
    res->setupResourceLoaders( ResourceLoaderGetType() );
}

//-----------------------------------//

void Engine::update()
{
    fldCore()->fileWatcher->update();
    fldCore()->resourceManager->update();

    AllocatorReset(fldCore()->stack);
}

//-----------------------------------//

NAMESPACE_ENGINE_END