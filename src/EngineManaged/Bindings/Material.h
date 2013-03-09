/************************************************************************
*
* Flood Project © (2008-201x)
* Licensed under the simplified BSD license. All rights reserved.
*
************************************************************************/

#pragma once

#include <Graphics/Resources/Material.h>
#include "Resource.h"
#include "ResourceHandle.h"
#include "Vector.h"
#include "ShaderMaterial.h"
#include "Image.h"

namespace Flood
{
    enum struct BlendSource;
    enum struct BlendDestination;
    enum struct DepthCompare;
    ref class Material;
    ref class Handle;
    ref class Allocator;
    ref class TextureUnit;
    ref class ShaderMaterial;
    enum struct ResourceGroup;
    value struct Vector2;
    ref class Image;

    public enum struct BlendSource
    {
        Zero = 0,
        One = 1,
        DestinationColor = 2,
        InverseDestinationColor = 3,
        SourceAlpha = 4,
        InverseSourceAlpha = 5,
        DestinationAlpha = 6,
        InverseDestinationAlpha = 7,
        SourceAlphaSaturate = 8
    };

    public enum struct BlendDestination
    {
        Zero = 0,
        One = 1,
        SourceColor = 2,
        InverseSourceColor = 3,
        SourceAlpha = 4,
        InverseSourceAlpha = 5,
        DestinationAlpha = 6,
        InverseDestinationAlpha = 7
    };

    public enum struct DepthCompare
    {
        Never = 0,
        Less = 1,
        Equal = 2,
        LessOrEqual = 3,
        Greater = 4,
        NotEqual = 5,
        GreaterOrEqual = 6,
        Always = 7
    };

    public ref class Material : Flood::Resource
    {
    public:
        Material(::Material* native);
        Material(System::IntPtr native);
        Material();
        Material(System::String^ name);
        property System::String^ Name;
        property Flood::ResourceHandle<Flood::ShaderMaterial^> Shader;
        property bool CullBackfaces;
        property Flood::DepthCompare DepthCompare;
        property bool DepthTest;
        property bool DepthWrite;
        property Flood::Vector2 DepthRange;
        property Flood::Vector2 DepthOffset;
        property bool AlphaTest;
        property bool LineSmooth;
        property float LineWidth;
        property Flood::BlendSource Source;
        property Flood::BlendDestination Destination;
        property bool _isBlendingEnabled;
        Flood::ResourceGroup GetResourceGroup();
        System::String^ GetName();
        void SetName(System::String^ v);
        Flood::ResourceHandle<Flood::ShaderMaterial^> GetShader();
        void SetShader(Flood::ResourceHandle<Flood::ShaderMaterial^> v);
        void SetShader(System::String^ name);
        bool GetDepthWrite();
        void SetDepthWrite(bool v);
        bool GetDepthTest();
        void SetDepthTest(bool v);
        Flood::DepthCompare GetDepthCompare();
        void SetDepthCompare(Flood::DepthCompare v);
        Flood::Vector2 GetDepthRange();
        void SetDepthRange(Flood::Vector2 v);
        Flood::Vector2 GetDepthOffset();
        void SetDepthOffset(Flood::Vector2 v);
        bool GetAlphaTest();
        void SetAlphaTest(bool v);
        float GetLineWidth();
        void SetLineWidth(float v);
        bool GetLineSmoothing();
        void SetLineSmoothing(bool v);
        bool GetBackfaceCulling();
        void SetBackfaceCulling(bool v);
        Flood::BlendSource GetBlendSource();
        Flood::BlendDestination GetBlendDestination();
        bool IsBlendingEnabled();
        void SetBlending(Flood::BlendSource _203, Flood::BlendDestination _204);
        void SetTexture(unsigned char unit, System::String^ name);
        void SetTexture(unsigned char unit, Flood::ResourceHandle<Flood::Image^> image);
        Flood::ResourceHandle<Flood::Image^> GetTexture(unsigned char unit);
        Flood::TextureUnit^ GetTextureUnit(unsigned char unit);
        void Init();
        static Flood::ResourceHandle<Flood::Material^> Create(Flood::Allocator^ _202, System::String^ name);
    };
}
