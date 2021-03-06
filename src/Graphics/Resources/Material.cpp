/************************************************************************
*
* Flood Project � (2008-201x)
* Licensed under the simplified BSD license. All rights reserved.
*
************************************************************************/

#include "Graphics/API.h"
#include "Graphics/Resources/Material.h"
#include "Resources/ResourceManager.h"
#include "Core/Log.h"

NAMESPACE_GRAPHICS_BEGIN

//-----------------------------------//

REFLECT_CHILD_CLASS(Material, Resource)
	FIELD_PRIMITIVE(4, string, name)
	FIELD_CLASS_PTR_SETTER(5, ShaderMaterial, ShaderMaterialHandle, shader, Handle, Shader)
	FIELD_PRIMITIVE(6, bool, cullBackfaces)
	FIELD_PRIMITIVE(7, bool, alphaTest)
	FIELD_PRIMITIVE(8, bool, lineSmooth)
	FIELD_PRIMITIVE(9, float, lineWidth)
	FIELD_PRIMITIVE(10, float, _isBlendingEnabled)
REFLECT_CLASS_END()

//-----------------------------------//

MaterialHandle MaterialCreate( Allocator* alloc, const String& name )
{
	Material* material = Allocate(alloc, Material);
	material->setName(name);

	return HandleCast<Material>( ResourceHandleCreate(material) );
}

float Material::DefaultLineWidth = 1.0f;

//-----------------------------------//

Material::Material()
{
	init();
	setShader("VertexColor");
}

//-----------------------------------//

Material::Material( const String& name )
	: name(name)
{
	init();
	setShader("VertexColor");
}

//-----------------------------------//

Material::Material( const Material& rhs )
	: name(rhs.name)
	, shader(rhs.shader)
	, cullBackfaces(rhs.cullBackfaces)
	, depthCompare(rhs.depthCompare)
	, depthTest(rhs.depthTest)
	, depthWrite(rhs.depthWrite)
	, depthRange(rhs.depthRange)
	, depthOffset(rhs.depthOffset)
	, lineSmooth(rhs.lineSmooth)
	, lineWidth(rhs.lineWidth)
	, source(rhs.source)
	, destination(rhs.destination)
	, _isBlendingEnabled(rhs._isBlendingEnabled)
	, textureUnits(rhs.textureUnits)
{
	init();
}

//-----------------------------------//

Material::~Material()
{
	LogDebug("Removing material '%s'", name.c_str());
}

//-----------------------------------//

void Material::init()
{
	alphaTest = false;
	depthCompare = DepthCompare::Less;
	depthRange = Vector2::UnitY;
	depthTest = true;
	depthWrite = true;
	depthOffset = Vector2::Zero;
	lineSmooth = false;
	cullBackfaces = true;
	lineWidth = DefaultLineWidth;
	_isBlendingEnabled = false;
	source = BlendSource::One;
	destination = BlendDestination::Zero;
}

//-----------------------------------//

bool Material::isBlendingEnabled() const
{
	return _isBlendingEnabled;
}

//-----------------------------------//

void Material::setBlending( BlendSource source, BlendDestination destination )
{
	this->source = source;
	this->destination = destination;
	
	_isBlendingEnabled = true;
}

//-----------------------------------//

TextureUnit::TextureUnit()
	: unit(0)
	, filter(TextureFilterMode::Linear)
	, mip(TextureMipMode::Nearest)
	, wrap(TextureWrapMode::Repeat)
	, overrideModes(false)
{
}

//-----------------------------------//

void TextureUnit::setFilterMode(TextureFilterMode newFilter)
{
	filter = newFilter;
	overrideModes = true;
}

//-----------------------------------//

void TextureUnit::setMipMode(TextureMipMode newMip)
{
	mip = newMip;
	overrideModes = true;
}

//-----------------------------------//

void TextureUnit::setWrapMode(TextureWrapMode newWrap)
{
	wrap = newWrap;
	overrideModes = true;
}

//-----------------------------------//

void Material::setTexture( uint8 unit, const String& name )
{
	ResourceManager* res = GetResourceManager();
	
	ImageHandle handle = res->loadResource<Image>(name);
	if( !handle ) return;

	TextureUnit texUnit;
	texUnit.image = handle;
	texUnit.unit = unit;

	textureUnits.set(unit, texUnit);
}

//-----------------------------------//

void Material::setTexture( uint8 unit, const ImageHandle& handle )
{
	auto tu = textureUnits.get(unit, TextureUnit());
	tu.image = handle;

	textureUnits.set(unit, tu);
}

//-----------------------------------//

ImageHandle Material::getTexture( uint8 unit )
{
	return textureUnits.get(unit, TextureUnit()).image;
}

//-----------------------------------//

TextureUnit& Material::getTextureUnit( uint8 unit )
{
	auto tu = textureUnits.get(unit, TextureUnit());
	if(!textureUnits.has(unit))
		textureUnits.set(unit, tu);
	tu = textureUnits.get(unit, tu);
	return tu;
}

//-----------------------------------//

void Material::setShader(const String& name)
{
	ResourceLoadOptions options;
	
	options.name = name;
	options.group = ResourceGroup::Shaders;

	// Use high priority so the shader is loaded right away.
	options.isHighPriority = true;
	options.asynchronousLoad = false;
	
	shader = HandleCast<ShaderMaterial>( GetResourceManager()->loadResource(options) );
}

//-----------------------------------//

NAMESPACE_GRAPHICS_END