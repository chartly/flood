/************************************************************************
*
* Flood Project � (2008-201x)
* Licensed under the simplified BSD license. All rights reserved.
*
************************************************************************/

#pragma once

#include "Core/API.h"
#include "Core/Event.h"
#include "Core/String.h"
#include "Core/Timer.h"

#include <miniformat/miniformat.h>

NAMESPACE_CORE_BEGIN

//-----------------------------------//

enum class LogLevel
{
    Info,
    Warn,
    Error,
    Debug,
    Assert
};

struct API_CORE LogEntry
{
    float time;
    String message;
    LogLevel level;
};

typedef void (*LogFunction)(LogEntry*);

class API_CORE Log
{
public:
    Log();
    ~Log();

    void addHandler(LogFunction handler);
    void removeHandler(LogFunction handler);
    void write(LogEntry* entry);

    Timer timer;
    Event1<LogEntry*> handlers;
};

API_CORE Log* LogGetDefault();
API_CORE void LogSetDefault(Log*);

API_CORE void LogInfo(const char* msg, ...);
API_CORE void LogWarn(const char* msg, ...);
API_CORE void LogError(const char* msg, ...);
API_CORE void LogDebug(const char* msg, ...);
API_CORE void LogAssert(const char* msg, ...);

//-----------------------------------//

NAMESPACE_CORE_END