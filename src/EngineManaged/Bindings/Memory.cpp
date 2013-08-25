/************************************************************************
*
* Flood Project © (2008-201x)
* Licensed under the simplified BSD license. All rights reserved.
*
************************************************************************/

#include "Memory.h"

using namespace System;
using namespace System::Runtime::InteropServices;

Flood::Allocator::Allocator(::Allocator* native)
{
    NativePtr = native;
}

Flood::Allocator::Allocator(System::IntPtr native)
{
    auto __native = (::Allocator*)native.ToPointer();
    NativePtr = __native;
}

Flood::Allocator::Allocator()
{
    NativePtr = new ::Allocator();
}

bool Flood::Allocator::Equals(System::Object^ object)
{
    if (!object) return false;
    auto obj = dynamic_cast<Allocator^>(object);

    if (!obj) return false;
    return Instance == obj->Instance;
}

int Flood::Allocator::GetHashCode()
{
    return (int)NativePtr;
}

void Flood::Allocator::Destroy()
{
    auto arg0 = (::Allocator*)NativePtr;
    ::AllocatorDestroy(arg0);
}

void Flood::Allocator::ResetMemory()
{
    auto arg0 = (::Allocator*)NativePtr;
    ::AllocatorReset(arg0);
}

void Flood::Allocator::SetGroup(System::String^ group)
{
    auto arg0 = (::Allocator*)NativePtr;
    auto _arg1 = clix::marshalString<clix::E_UTF8>(group);
    auto arg1 = _arg1.c_str();
    ::AllocatorSetGroup(arg0, arg1);
}

Flood::Allocator^ Flood::Allocator::CreatePool(int size)
{
    auto arg0 = (::Allocator*)NativePtr;
    auto arg1 = (::int32)size;
    auto __ret = ::AllocatorCreatePool(arg0, arg1);
    return gcnew Flood::Allocator((::Allocator*)__ret);
}

Flood::Allocator^ Flood::Allocator::CreateBump(int size)
{
    auto arg0 = (::Allocator*)NativePtr;
    auto arg1 = (::int32)size;
    auto __ret = ::AllocatorCreateBump(arg0, arg1);
    return gcnew Flood::Allocator((::Allocator*)__ret);
}

Flood::Allocator^ Flood::Allocator::CreateHeap()
{
    auto arg0 = (::Allocator*)NativePtr;
    auto __ret = ::AllocatorCreateHeap(arg0);
    return gcnew Flood::Allocator((::Allocator*)__ret);
}

Flood::Allocator^ Flood::Allocator::CreateStack()
{
    auto arg0 = (::Allocator*)NativePtr;
    auto __ret = ::AllocatorCreateStack(arg0);
    return gcnew Flood::Allocator((::Allocator*)__ret);
}

Flood::Allocator^ Flood::Allocator::GetHeap()
{
    auto __ret = ::AllocatorGetHeap();
    return gcnew Flood::Allocator((::Allocator*)__ret);
}

Flood::Allocator^ Flood::Allocator::GetStack()
{
    auto __ret = ::AllocatorGetStack();
    return gcnew Flood::Allocator((::Allocator*)__ret);
}

Flood::Allocator^ Flood::Allocator::GetObject(System::IntPtr _0)
{
    auto arg0 = (void*)_0.ToPointer();
    auto __ret = ::AllocatorGetObject(arg0);
    return gcnew Flood::Allocator((::Allocator*)__ret);
}

void Flood::Allocator::DumpInfo()
{
    ::AllocatorDumpInfo();
}

System::IntPtr Flood::Allocator::Instance::get()
{
    return System::IntPtr(NativePtr);
}

void Flood::Allocator::Instance::set(System::IntPtr object)
{
    NativePtr = (::Allocator*)object.ToPointer();
}

Flood::MemoryAllocateFunction^ Flood::Allocator::Allocate::get()
{
    return safe_cast<Flood::MemoryAllocateFunction^>(System::Runtime::InteropServices::Marshal::GetDelegateForFunctionPointer(IntPtr(((::Allocator*)NativePtr)->allocate), Flood::MemoryAllocateFunction::typeid));
}

void Flood::Allocator::Allocate::set(Flood::MemoryAllocateFunction^ value)
{
    ((::Allocator*)NativePtr)->allocate = static_cast<::MemoryAllocateFunction>(System::Runtime::InteropServices::Marshal::GetFunctionPointerForDelegate(value).ToPointer());
}

Flood::MemoryFreeFunction^ Flood::Allocator::Deallocate::get()
{
    return safe_cast<Flood::MemoryFreeFunction^>(System::Runtime::InteropServices::Marshal::GetDelegateForFunctionPointer(IntPtr(((::Allocator*)NativePtr)->deallocate), Flood::MemoryFreeFunction::typeid));
}

void Flood::Allocator::Deallocate::set(Flood::MemoryFreeFunction^ value)
{
    ((::Allocator*)NativePtr)->deallocate = static_cast<::MemoryFreeFunction>(System::Runtime::InteropServices::Marshal::GetFunctionPointerForDelegate(value).ToPointer());
}

Flood::MemoryResetFunction^ Flood::Allocator::Reset::get()
{
    return safe_cast<Flood::MemoryResetFunction^>(System::Runtime::InteropServices::Marshal::GetDelegateForFunctionPointer(IntPtr(((::Allocator*)NativePtr)->reset), Flood::MemoryResetFunction::typeid));
}

void Flood::Allocator::Reset::set(Flood::MemoryResetFunction^ value)
{
    ((::Allocator*)NativePtr)->reset = static_cast<::MemoryResetFunction>(System::Runtime::InteropServices::Marshal::GetFunctionPointerForDelegate(value).ToPointer());
}

System::String^ Flood::Allocator::Group::get()
{
    return clix::marshalString<clix::E_UTF8>(((::Allocator*)NativePtr)->group);
}

void Flood::Allocator::Group::set(System::String^ value)
{
    auto _value = clix::marshalString<clix::E_UTF8>(value);
    ((::Allocator*)NativePtr)->group = _value.c_str();
}

Flood::PoolAllocator::PoolAllocator(::PoolAllocator* native)
    : Flood::Allocator((::Allocator*)native)
{
}

Flood::PoolAllocator::PoolAllocator(System::IntPtr native)
    : Flood::Allocator(native)
{
    auto __native = (::PoolAllocator*)native.ToPointer();
}

Flood::PoolAllocator::PoolAllocator()
    : Flood::Allocator((::Allocator*)nullptr)
{
    NativePtr = new ::PoolAllocator();
}

bool Flood::PoolAllocator::Equals(System::Object^ object)
{
    if (!object) return false;
    auto obj = dynamic_cast<PoolAllocator^>(object);

    if (!obj) return false;
    return Instance == obj->Instance;
}

int Flood::PoolAllocator::GetHashCode()
{
    return (int)NativePtr;
}

System::IntPtr Flood::PoolAllocator::Current::get()
{
    return IntPtr(((::PoolAllocator*)NativePtr)->current);
}

void Flood::PoolAllocator::Current::set(System::IntPtr value)
{
    ((::PoolAllocator*)NativePtr)->current = (::uint8*)value.ToPointer();
}

Flood::BumpAllocator::BumpAllocator(::BumpAllocator* native)
    : Flood::Allocator((::Allocator*)native)
{
}

Flood::BumpAllocator::BumpAllocator(System::IntPtr native)
    : Flood::Allocator(native)
{
    auto __native = (::BumpAllocator*)native.ToPointer();
}

Flood::BumpAllocator::BumpAllocator()
    : Flood::Allocator((::Allocator*)nullptr)
{
    NativePtr = new ::BumpAllocator();
}

bool Flood::BumpAllocator::Equals(System::Object^ object)
{
    if (!object) return false;
    auto obj = dynamic_cast<BumpAllocator^>(object);

    if (!obj) return false;
    return Instance == obj->Instance;
}

int Flood::BumpAllocator::GetHashCode()
{
    return (int)NativePtr;
}

System::IntPtr Flood::BumpAllocator::Start::get()
{
    return IntPtr(((::BumpAllocator*)NativePtr)->start);
}

void Flood::BumpAllocator::Start::set(System::IntPtr value)
{
    ((::BumpAllocator*)NativePtr)->start = (::uint8*)value.ToPointer();
}

System::IntPtr Flood::BumpAllocator::Current::get()
{
    return IntPtr(((::BumpAllocator*)NativePtr)->current);
}

void Flood::BumpAllocator::Current::set(System::IntPtr value)
{
    ((::BumpAllocator*)NativePtr)->current = (::uint8*)value.ToPointer();
}

unsigned int Flood::BumpAllocator::Size::get()
{
    return ((::BumpAllocator*)NativePtr)->size;
}

void Flood::BumpAllocator::Size::set(unsigned int value)
{
    ((::BumpAllocator*)NativePtr)->size = (::uint32)value;
}

Flood::HeapAllocator::HeapAllocator(::HeapAllocator* native)
    : Flood::Allocator((::Allocator*)native)
{
}

Flood::HeapAllocator::HeapAllocator(System::IntPtr native)
    : Flood::Allocator(native)
{
    auto __native = (::HeapAllocator*)native.ToPointer();
}

Flood::HeapAllocator::HeapAllocator()
    : Flood::Allocator((::Allocator*)nullptr)
{
    NativePtr = new ::HeapAllocator();
}

bool Flood::HeapAllocator::Equals(System::Object^ object)
{
    if (!object) return false;
    auto obj = dynamic_cast<HeapAllocator^>(object);

    if (!obj) return false;
    return Instance == obj->Instance;
}

int Flood::HeapAllocator::GetHashCode()
{
    return (int)NativePtr;
}

System::IntPtr Flood::HeapAllocator::Space::get()
{
    return IntPtr(((::HeapAllocator*)NativePtr)->space);
}

void Flood::HeapAllocator::Space::set(System::IntPtr value)
{
    ((::HeapAllocator*)NativePtr)->space = (void*)value.ToPointer();
}

