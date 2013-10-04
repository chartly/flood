/************************************************************************
*
* Flood Project © (2008-201x)
* Licensed under the simplified BSD license. All rights reserved.
*
************************************************************************/

#pragma once

NAMESPACE_CORE_BEGIN

//-----------------------------------// 

struct Allocator;
struct Log;
class Stream;
class TaskPool;
class FileWatcher;

//-----------------------------------//

/**
 *	Core systems container class. This singleton is a means by which the
 *	various core systems in the engine can control their order of initialization,
 *	provide a central location for accessing various global allocator schemes,
 *	and more clearly illustrate the hierarchy of ownership in the game engine.
 */

class API_CORE Core
{
public:
    static void init();
    static void shutdown();
    static Core* get();

    Allocator* heap;
    Allocator* stack;
    Log* log;
    Stream* logStream;
    TaskPool* taskPool;
    FileWatcher* fileWatcher;

private:
    Core();
    Core(const Core& rhs);
    Core& operator=(const Core& rhs);

    static Core* s_Core;
};

API_CORE Core* fldCore();

NAMESPACE_CORE_END