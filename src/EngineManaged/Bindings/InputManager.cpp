/************************************************************************
*
* Flood Project © (2008-201x)
* Licensed under the simplified BSD license. All rights reserved.
*
************************************************************************/

#include "InputManager.h"
#include "Device.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "Window.h"

using namespace System;
using namespace System::Runtime::InteropServices;

Flood::InputManager::InputManager(::InputManager* native)
{
    NativePtr = native;
}

Flood::InputManager::InputManager(System::IntPtr native)
{
    auto __native = (::InputManager*)native.ToPointer();
    NativePtr = __native;
}

Flood::InputManager::InputManager()
{
}

bool Flood::InputManager::Equals(System::Object^ object)
{
    if (!object) return false;
    auto obj = dynamic_cast<InputManager^>(object);

    if (!obj) return false;
    return Instance == obj->Instance;
}

int Flood::InputManager::GetHashCode()
{
    return (int)NativePtr;
}

System::IntPtr Flood::InputManager::Instance::get()
{
    return System::IntPtr(NativePtr);
}

void Flood::InputManager::Instance::set(System::IntPtr object)
{
    NativePtr = (::InputManager*)object.ToPointer();
}

Flood::Keyboard^ Flood::InputManager::Keyboard::get()
{
    auto __ret = ((::InputManager*)NativePtr)->getKeyboard();
    if (__ret == nullptr) return nullptr;
    return gcnew Flood::Keyboard((::Keyboard*)__ret);
}

Flood::Mouse^ Flood::InputManager::Mouse::get()
{
    auto __ret = ((::InputManager*)NativePtr)->getMouse();
    if (__ret == nullptr) return nullptr;
    return gcnew Flood::Mouse((::Mouse*)__ret);
}

void Flood::FloodInputManager::InputInitialize()
{
    ::InputInitialize();
}

void Flood::FloodInputManager::InputDeinitialize()
{
    ::InputDeinitialize();
}

Flood::InputManager^ Flood::FloodInputManager::GetInputManager()
{
    auto __ret = ::GetInputManager();
    if (__ret == nullptr) return nullptr;
    return gcnew Flood::InputManager((::InputManager*)__ret);
}

