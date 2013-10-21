/************************************************************************
*
* Flood Project © (2008-201x)
* Licensed under the simplified BSD license. All rights reserved.
*
************************************************************************/

#include "Core/API.h"
#include "Core/Log.h"
#include "Core/Memory.h"
#include "Core/Stream.h"
#include "Core/Task.h"

#ifdef PLATFORM_WINDOWS
    #include "Core/FileWatcherWin32.h"
#else
    #include "Core/FileWatcherNull.h"
#endif

#ifdef API_CORE_DLL_EXPORT
FL_INSTANTIATE_TEMPLATES()
#endif

NAMESPACE_CORE_BEGIN
Core* Core::s_Core = nullptr;

Core::Core()
    : heap(nullptr)
    , stack(nullptr)
    , log(nullptr)
    , logStream(nullptr)
    , taskPool(nullptr)
    , fileWatcher(nullptr)
    , resourceManager(nullptr)
{}

void Core::init()
{
    assert(s_Core == nullptr);

#pragma TODO("Alloc core systems in a more sane place than just on the heap.")
    // init the singleton
    auto gHeap = AllocatorGetHeap();
    s_Core = Allocate(gHeap, Core);
    
    assert(s_Core);

    // init memory system
    s_Core->heap = gHeap;
    s_Core->stack = AllocatorGetStack();

    // init the main logger.
    s_Core->logStream = Allocate(s_Core->heap, FileStream, "Log.html", StreamOpenMode::Write);
    s_Core->log = Allocate(s_Core->heap, Log);

    LogInfo("Core is initializing...");
    LogInfo("Log created...");

    // init the task pool
    s_Core->taskPool = Allocate(s_Core->heap, TaskPool, 4);

    // init the file watcher
    LogInfo("File watcher created...");
#ifdef PLATFORM_WINDOWS
    s_Core->fileWatcher = AllocateHeap(FileWatcherWin32);
#else
    s_Core->fileWatcher = AllocateHeap(FileWatcherNull);
#endif
}

void Core::shutdown()
{
    LogInfo("Core shutting down...");

    Deallocate(s_Core->fileWatcher);
    Deallocate(s_Core->taskPool);
    Deallocate(s_Core->log);
    Deallocate(s_Core->logStream);
    s_Core->stack = nullptr;
    s_Core->heap = nullptr;
}

Core* Core::get()
{
    return s_Core;
}

Core* fldCore()
{
    return Core::get();
}

NAMESPACE_CORE_END