/************************************************************************
*
* Flood Project © (2008-201x)
* Licensed under the simplified BSD license. All rights reserved.
*
************************************************************************/

#pragma once

#include <Core/Log.h>
#include "ResourceHandle.h"

namespace Flood
{
    enum struct LogLevel;
    ref class Log;
    ref class Allocator;
    value struct LogEntry;
    ref class Timer;
    ref class Mutex;
    ref class Event1;
    enum struct TypeAttributeKind : unsigned char;
    enum struct TypeKind : unsigned char;
    enum struct ReflectionWalkType : unsigned char;
    enum struct FieldQualifier : unsigned short;
    enum struct PrimitiveTypeKind : unsigned char;
    ref class Type;
    ref class ReflectionDatabase;
    ref class Field;
    ref class PrimitiveBuiltins;
    ref class Enum;
    ref class ReflectionContext;
    ref class Primitive;

    public enum struct LogLevel
    {
        Info = 0,
        Warn = 1,
        Error = 2,
        Debug = 3,
        Assert = 4
    };

    public delegate void LogFunction(Flood::LogEntry);

    public value struct LogEntry
    {
    public:
        LogEntry(::LogEntry* native);
        LogEntry(System::IntPtr native);
        float Time;
        System::String^ Message;
        Flood::LogLevel Level;
    };

    public ref class Log
    {
    public:
        property ::Log* NativePtr;

        Log(::Log* native);
        Log(System::IntPtr native);
        Log();
    private:
        delegate void _HandlersDelegate(::LogEntry* _0);
        _HandlersDelegate^ _HandlersDelegateInstance;
        void _HandlersRaise(::LogEntry* _0);
        System::Action<Flood::LogEntry>^ _Handlers;
    public:
        event System::Action<Flood::LogEntry>^ Handlers
        {
            void add(System::Action<Flood::LogEntry>^ evt);
            void remove(System::Action<Flood::LogEntry>^ evt);
            void raise(Flood::LogEntry);
        }
        void Destroy();
        void AddHandler(Flood::LogFunction^ _4);
        void RemoveHandler(Flood::LogFunction^ _6);
        void Write(Flood::LogEntry entry);
        void SetDefault();
        static Flood::Log^ GetDefault();
        static void Info(System::String^ msg);
        static void Warn(System::String^ msg);
        static void Error(System::String^ msg);
        static void Debug(System::String^ msg);
        static void Assert(System::String^ msg);
    };
}
