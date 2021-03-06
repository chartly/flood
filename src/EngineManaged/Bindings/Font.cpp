/************************************************************************
*
* Flood Project © (2008-201x)
* Licensed under the simplified BSD license. All rights reserved.
*
************************************************************************/

#include "Font.h"
#include "Resource.h"
#include "Vector.h"

using namespace System;
using namespace System::Runtime::InteropServices;

Flood::Glyph::Glyph(::Glyph* native)
{
    XOffset = native->xOffset;
    BaseLineOffset = native->baseLineOffset;
    Advance = native->advance;
    Width = native->width;
    Height = native->height;
}

Flood::Glyph::Glyph(System::IntPtr native)
{
    auto __native = (::Glyph*)native.ToPointer();
    XOffset = __native->xOffset;
    BaseLineOffset = __native->baseLineOffset;
    Advance = __native->advance;
    Width = __native->width;
    Height = __native->height;
}

Flood::Font::Font(::Font* native)
    : Flood::Resource((::Resource*)native)
{
}

Flood::Font::Font(System::IntPtr native)
    : Flood::Resource(native)
{
    auto __native = (::Font*)native.ToPointer();
}

bool Flood::Font::GetGlyphInfo(int codepoint, int size, [System::Runtime::InteropServices::Out] Flood::Glyph% glyph)
{
    ::Glyph arg2;
    auto __ret = ((::Font*)NativePtr)->getGlyphInfo(codepoint, size, arg2);
    glyph = Flood::Glyph((::Glyph*)&arg2);
    return __ret;
}

Flood::ResourceHandle<Flood::Image^> Flood::Font::CreateGlyphImage(int codepoint, int size)
{
    auto __ret = ((::Font*)NativePtr)->createGlyphImage(codepoint, size);
    return Flood::ResourceHandle<Flood::Image^>(__ret.id);
}

Flood::Vector2 Flood::Font::GetKerning(int codepoint1, int codepoint2, int fontSize)
{
    auto __ret = ((::Font*)NativePtr)->getKerning(codepoint1, codepoint2, fontSize);
    return Flood::Vector2((::Vector2*)&__ret);
}

bool Flood::Font::Equals(System::Object^ object)
{
    if (!object) return false;
    auto obj = dynamic_cast<Font^>(object);

    if (!obj) return false;
    return Instance == obj->Instance;
}

int Flood::Font::GetHashCode()
{
    return (int)NativePtr;
}

System::String^ Flood::Font::Name::get()
{
    auto &__ret = ((::Font*)NativePtr)->getName();
    return clix::marshalString<clix::E_UTF8>(__ret);
}

void Flood::Font::Name::set(System::String^ value)
{
    auto v = value;
    auto arg0 = clix::marshalString<clix::E_UTF8>(v);
    ((::Font*)NativePtr)->setName(arg0);
}

Flood::ResourceGroup Flood::Font::ResourceGroup::get()
{
    auto __ret = ((::Font*)NativePtr)->getResourceGroup();
    return (Flood::ResourceGroup)__ret;
}

System::String^ Flood::Font::Name1::get()
{
    return clix::marshalString<clix::E_UTF8>(((::Font*)NativePtr)->name);
}

void Flood::Font::Name1::set(System::String^ value)
{
    ((::Font*)NativePtr)->name = clix::marshalString<clix::E_UTF8>(value);
}

