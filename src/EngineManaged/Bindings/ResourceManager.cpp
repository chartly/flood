/************************************************************************
*
* Flood Project © (2008-201x)
* Licensed under the simplified BSD license. All rights reserved.
*
************************************************************************/

#include "_Marshal.h"
#include "ResourceManager.h"
#include "ResourceLoader.h"
#include "Stream.h"
#include "Memory.h"
#include "Resource.h"
#include "Reflection.h"
#include "Serialization.h"
#include "Extension.h"

using namespace System;
using namespace System::Runtime::InteropServices;
using namespace clix;

Flood::ResourceEvent::ResourceEvent(::ResourceEvent* native)
{
    Resource = gcnew Flood::Resource((::Resource*)native->resource);
    OldResource = gcnew Flood::Resource((::Resource*)native->oldResource);
    Handle = native->handle.id;
}

Flood::ResourceEvent::ResourceEvent(System::IntPtr native)
{
    auto __native = (::ResourceEvent*)native.ToPointer();
    Resource = gcnew Flood::Resource((::Resource*)__native->resource);
    OldResource = gcnew Flood::Resource((::Resource*)__native->oldResource);
    Handle = __native->handle.id;
}

Flood::ResourceManager::ResourceManager(::ResourceManager* native)
{
    NativePtr = native;
}

Flood::ResourceManager::ResourceManager(System::IntPtr native)
{
    auto __native = (::ResourceManager*)native.ToPointer();
    NativePtr = __native;
}

Flood::ResourceManager::ResourceManager()
{
    NativePtr = new ::ResourceManager();
}

uint Flood::ResourceManager::GetResource(System::String^ name)
{
    auto arg0 = marshalString<E_UTF8>(name);
    auto ret = ((::ResourceManager*)NativePtr)->getResource(arg0);
    return ret.id;
}

uint Flood::ResourceManager::LoadResource(System::String^ name)
{
    auto arg0 = marshalString<E_UTF8>(name);
    auto ret = ((::ResourceManager*)NativePtr)->loadResource(arg0);
    return ret.id;
}

uint Flood::ResourceManager::LoadResource(Flood::ResourceLoadOptions options)
{
    auto arg0 = ::ResourceLoadOptions();
    arg0.name = marshalString<E_UTF8>(options.Name);
    arg0.stream = (::Stream*)options.Stream->NativePtr;
    arg0.resource = (::Resource*)options.Resource->NativePtr;
    arg0.group = (::ResourceGroup)options.Group;
    arg0.isHighPriority = options.IsHighPriority;
    arg0.sendLoadEvent = options.SendLoadEvent;
    arg0.asynchronousLoad = options.AsynchronousLoad;
    arg0.keepStreamOpen = options.KeepStreamOpen;
    arg0.option = ::ResourceLoadOption();

    auto ret = ((::ResourceManager*)NativePtr)->loadResource(arg0);
    return ret.id;
}

bool Flood::ResourceManager::FindResource(Flood::ResourceLoadOptions options)
{
    auto arg0 = ::ResourceLoadOptions();
    arg0.name = marshalString<E_UTF8>(options.Name);
    arg0.stream = (::Stream*)options.Stream->NativePtr;
    arg0.resource = (::Resource*)options.Resource->NativePtr;
    arg0.group = (::ResourceGroup)options.Group;
    arg0.isHighPriority = options.IsHighPriority;
    arg0.sendLoadEvent = options.SendLoadEvent;
    arg0.asynchronousLoad = options.AsynchronousLoad;
    arg0.keepStreamOpen = options.KeepStreamOpen;
    arg0.option = ::ResourceLoadOption();

    auto ret = ((::ResourceManager*)NativePtr)->findResource(arg0);
    return ret;
}

void Flood::ResourceManager::RemoveResource(Flood::Resource^ resource)
{
    auto arg0 = (::Resource*)resource->NativePtr;
    ((::ResourceManager*)NativePtr)->removeResource(arg0);
}

void Flood::ResourceManager::RemoveResource(System::String^ name)
{
    auto arg0 = marshalString<E_UTF8>(name);
    ((::ResourceManager*)NativePtr)->removeResource(arg0);
}

void Flood::ResourceManager::RemoveUnusedResources()
{
    ((::ResourceManager*)NativePtr)->removeUnusedResources();
}

void Flood::ResourceManager::LoadQueuedResources()
{
    ((::ResourceManager*)NativePtr)->loadQueuedResources();
}

void Flood::ResourceManager::Update()
{
    ((::ResourceManager*)NativePtr)->update();
}

Flood::ResourceLoader^ Flood::ResourceManager::FindLoader(System::String^ extension)
{
    auto arg0 = marshalString<E_UTF8>(extension);
    auto ret = ((::ResourceManager*)NativePtr)->findLoader(arg0);
    return gcnew Flood::ResourceLoader((::ResourceLoader*)ret);
}

Flood::ResourceLoader^ Flood::ResourceManager::FindLoaderByClass(Flood::Class^ klass)
{
    auto arg0 = (::Class*)klass->NativePtr;
    auto ret = ((::ResourceManager*)NativePtr)->findLoaderByClass(arg0);
    return gcnew Flood::ResourceLoader((::ResourceLoader*)ret);
}

void Flood::ResourceManager::SetupResourceLoaders(Flood::Class^ klass)
{
    auto arg0 = (::Class*)klass->NativePtr;
    ((::ResourceManager*)NativePtr)->setupResourceLoaders(arg0);
}

bool Flood::ResourceManager::GetAsynchronousLoading()
{
    auto ret = ((::ResourceManager*)NativePtr)->getAsynchronousLoading();
    return ret;
}

void Flood::ResourceManager::SetAsynchronousLoading(bool v)
{
    ((::ResourceManager*)NativePtr)->setAsynchronousLoading(v);
}

void Flood::ResourceManager::ResourcePrepared::add(System::Action<Flood::ResourceEvent>^ evt)
{
    if (!_ResourcePreparedDelegateInstance)
    {
        _ResourcePreparedDelegateInstance = gcnew _ResourcePreparedDelegate(this, &Flood::ResourceManager::_ResourcePreparedRaise);
        auto _fptr = (void (*)(const ::ResourceEvent&))Marshal::GetFunctionPointerForDelegate(_ResourcePreparedDelegateInstance).ToPointer();
        ((::ResourceManager*)NativePtr)->onResourcePrepared.Connect(_fptr);
    }
    _ResourcePrepared = static_cast<System::Action<Flood::ResourceEvent>^>(System::Delegate::Combine(_ResourcePrepared, evt));
}

void Flood::ResourceManager::ResourcePrepared::remove(System::Action<Flood::ResourceEvent>^ evt)
{
    _ResourcePrepared = static_cast<System::Action<Flood::ResourceEvent>^>(System::Delegate::Remove(_ResourcePrepared, evt));
}

void Flood::ResourceManager::ResourcePrepared::raise(Flood::ResourceEvent _0)
{
    _ResourcePrepared(_0);
}

void Flood::ResourceManager::_ResourcePreparedRaise(const ::ResourceEvent& _0)
{
    ResourcePrepared::raise(Flood::ResourceEvent((::ResourceEvent*)&_0));
}

void Flood::ResourceManager::ResourceLoaded::add(System::Action<Flood::ResourceEvent>^ evt)
{
    if (!_ResourceLoadedDelegateInstance)
    {
        _ResourceLoadedDelegateInstance = gcnew _ResourceLoadedDelegate(this, &Flood::ResourceManager::_ResourceLoadedRaise);
        auto _fptr = (void (*)(const ::ResourceEvent&))Marshal::GetFunctionPointerForDelegate(_ResourceLoadedDelegateInstance).ToPointer();
        ((::ResourceManager*)NativePtr)->onResourceLoaded.Connect(_fptr);
    }
    _ResourceLoaded = static_cast<System::Action<Flood::ResourceEvent>^>(System::Delegate::Combine(_ResourceLoaded, evt));
}

void Flood::ResourceManager::ResourceLoaded::remove(System::Action<Flood::ResourceEvent>^ evt)
{
    _ResourceLoaded = static_cast<System::Action<Flood::ResourceEvent>^>(System::Delegate::Remove(_ResourceLoaded, evt));
}

void Flood::ResourceManager::ResourceLoaded::raise(Flood::ResourceEvent _0)
{
    _ResourceLoaded(_0);
}

void Flood::ResourceManager::_ResourceLoadedRaise(const ::ResourceEvent& _0)
{
    ResourceLoaded::raise(Flood::ResourceEvent((::ResourceEvent*)&_0));
}

void Flood::ResourceManager::ResourceRemoved::add(System::Action<Flood::ResourceEvent>^ evt)
{
    if (!_ResourceRemovedDelegateInstance)
    {
        _ResourceRemovedDelegateInstance = gcnew _ResourceRemovedDelegate(this, &Flood::ResourceManager::_ResourceRemovedRaise);
        auto _fptr = (void (*)(const ::ResourceEvent&))Marshal::GetFunctionPointerForDelegate(_ResourceRemovedDelegateInstance).ToPointer();
        ((::ResourceManager*)NativePtr)->onResourceRemoved.Connect(_fptr);
    }
    _ResourceRemoved = static_cast<System::Action<Flood::ResourceEvent>^>(System::Delegate::Combine(_ResourceRemoved, evt));
}

void Flood::ResourceManager::ResourceRemoved::remove(System::Action<Flood::ResourceEvent>^ evt)
{
    _ResourceRemoved = static_cast<System::Action<Flood::ResourceEvent>^>(System::Delegate::Remove(_ResourceRemoved, evt));
}

void Flood::ResourceManager::ResourceRemoved::raise(Flood::ResourceEvent _0)
{
    _ResourceRemoved(_0);
}

void Flood::ResourceManager::_ResourceRemovedRaise(const ::ResourceEvent& _0)
{
    ResourceRemoved::raise(Flood::ResourceEvent((::ResourceEvent*)&_0));
}

void Flood::ResourceManager::ResourceReloaded::add(System::Action<Flood::ResourceEvent>^ evt)
{
    if (!_ResourceReloadedDelegateInstance)
    {
        _ResourceReloadedDelegateInstance = gcnew _ResourceReloadedDelegate(this, &Flood::ResourceManager::_ResourceReloadedRaise);
        auto _fptr = (void (*)(const ::ResourceEvent&))Marshal::GetFunctionPointerForDelegate(_ResourceReloadedDelegateInstance).ToPointer();
        ((::ResourceManager*)NativePtr)->onResourceReloaded.Connect(_fptr);
    }
    _ResourceReloaded = static_cast<System::Action<Flood::ResourceEvent>^>(System::Delegate::Combine(_ResourceReloaded, evt));
}

void Flood::ResourceManager::ResourceReloaded::remove(System::Action<Flood::ResourceEvent>^ evt)
{
    _ResourceReloaded = static_cast<System::Action<Flood::ResourceEvent>^>(System::Delegate::Remove(_ResourceReloaded, evt));
}

void Flood::ResourceManager::ResourceReloaded::raise(Flood::ResourceEvent _0)
{
    _ResourceReloaded(_0);
}

void Flood::ResourceManager::_ResourceReloadedRaise(const ::ResourceEvent& _0)
{
    ResourceReloaded::raise(Flood::ResourceEvent((::ResourceEvent*)&_0));
}

void Flood::ResourceManager::ResourceLoaderRegistered::add(System::Action<Flood::ResourceLoader^>^ evt)
{
    if (!_ResourceLoaderRegisteredDelegateInstance)
    {
        _ResourceLoaderRegisteredDelegateInstance = gcnew _ResourceLoaderRegisteredDelegate(this, &Flood::ResourceManager::_ResourceLoaderRegisteredRaise);
        auto _fptr = (void (*)(const ::ResourceLoader&))Marshal::GetFunctionPointerForDelegate(_ResourceLoaderRegisteredDelegateInstance).ToPointer();
        ((::ResourceManager*)NativePtr)->onResourceLoaderRegistered.Connect(_fptr);
    }
    _ResourceLoaderRegistered = static_cast<System::Action<Flood::ResourceLoader^>^>(System::Delegate::Combine(_ResourceLoaderRegistered, evt));
}

void Flood::ResourceManager::ResourceLoaderRegistered::remove(System::Action<Flood::ResourceLoader^>^ evt)
{
    _ResourceLoaderRegistered = static_cast<System::Action<Flood::ResourceLoader^>^>(System::Delegate::Remove(_ResourceLoaderRegistered, evt));
}

void Flood::ResourceManager::ResourceLoaderRegistered::raise(Flood::ResourceLoader^ _0)
{
    _ResourceLoaderRegistered(_0);
}

void Flood::ResourceManager::_ResourceLoaderRegisteredRaise(const ::ResourceLoader& _0)
{
    ResourceLoaderRegistered::raise(gcnew Flood::ResourceLoader((::ResourceLoader*)&_0));
}

Flood::ResourceManager^ Flood::FloodResourceManager::GetResourceManager()
{
    auto ret = ::GetResourceManager();
    return gcnew Flood::ResourceManager((::ResourceManager*)ret);
}

