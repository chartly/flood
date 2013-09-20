/************************************************************************
*
* Flood Project © (2008-201x)
* Licensed under the simplified BSD license. All rights reserved.
*
************************************************************************/

#include "Graphics/API.h"
#include "Graphics/TextureManager.h"
#include "Graphics/RenderBackend.h"
#include "Resources/ResourceManager.h"

NAMESPACE_GRAPHICS_BEGIN

//-----------------------------------//

TextureManager::TextureManager()
{
	ResourceManager* res = GetResourceManager();
	res->onResourceLoaded.Connect( this, &TextureManager::onLoaded );
	res->onResourceRemoved.Connect( this, &TextureManager::onUnloaded );
	res->onResourceReloaded.Connect( this, &TextureManager::onReloaded );
}

//-----------------------------------//

TextureManager::~TextureManager()
{
	ResourceManager* res = GetResourceManager();
	res->onResourceLoaded.Disconnect( this, &TextureManager::onLoaded );
	res->onResourceRemoved.Disconnect( this, &TextureManager::onUnloaded );
	res->onResourceReloaded.Disconnect( this, &TextureManager::onReloaded );

	#pragma TODO("Make sure all textures are released on exit")
	for( auto it = textures.begin(); it != textures.end(); it++ )
	{
		const Texture* texture = it->value.get();
		//assert( texture->getReferenceCount() == 2 );
	}
}

//-----------------------------------//

void TextureManager::removeTexture(Image* image)
{
	textures.remove((uint64)image);
}

//-----------------------------------//

static const uint8 TEX_SIZE = 64;

TexturePtr TextureManager::getTexture( const String& name )
{
	ResourceManager* res = GetResourceManager();
	ImageHandle handle = res->loadResource<Image>(name);
	return getTexture(handle);
}

//-----------------------------------//

TexturePtr TextureManager::getTexture( Image* image )
{
	if( !image )
		return nullptr;

	auto tex = textures.get((uint64)image, nullptr);

	if( textures.has((uint64)image) && !image->isLoaded() ) 
	{
		tex = backend->createTexture();
		tex->allocate(Vector2i(TEX_SIZE, TEX_SIZE), PixelFormat::R8G8B8A8);

		textures.set((uint64)image, tex);
	}

	return tex;
}

//-----------------------------------//

TexturePtr TextureManager::getTexture( const ImageHandle& imageHandle )
{
	Image* image = imageHandle.Resolve();

	if (TexturePtr texture = getTexture(image))
		return texture;

	// Create a new texture from image.
	Texture* texture = backend->createTexture();
	texture->setImage(imageHandle);

	textures.set((uint64)image, texture);

	return texture;
}

//-----------------------------------//

void TextureManager::onLoaded( const ResourceEvent& event )
{
	ImageHandle handleImage = HandleCast<Image>(event.handle);
	Image* image = handleImage.Resolve();

	if( image->getResourceGroup() != ResourceGroup::Images )
		return;

	if( !textures.has((uint64)image) )
		return;

	auto tex = textures.get((uint64)image, nullptr);
	tex->setImage(handleImage);

	backend->uploadTexture(tex.get());
}

//-----------------------------------//

void TextureManager::onUnloaded( const ResourceEvent& event )
{
	ImageHandle handleImage = HandleCast<Image>(event.handle);
	Image* image = handleImage.Resolve();

	if( image->getResourceGroup() != ResourceGroup::Images )
		return;

	if( !textures.has((uint64)image) )
		return;

	LogDebug( "Removing texture '%s'", image->getPath().c_str() );

	removeTexture(image);
}

//-----------------------------------//

void TextureManager::onReloaded( const ResourceEvent& event )
{
	ImageHandle handleImage = HandleCast<Image>(event.handle);
	Image* newImage = handleImage.Resolve();

	if( newImage->getResourceGroup() != ResourceGroup::Images )
		return;
	
	Image* oldImage = (Image*) event.oldResource;

	if( !textures.has((uint64)oldImage) )
		return;

	LogDebug( "Reloading texture '%s'", newImage->getPath().c_str() );

	auto tex = textures.get((uint64)oldImage, nullptr);
	tex->setImage(handleImage);

	textures.remove((uint64)oldImage);
	textures.set((uint64)newImage, tex);
}

//-----------------------------------//

uint TextureManager::getMemoryUsage()
{
	uint total = 0;

	for( auto it = textures.begin(); it != textures.end(); it++ )
	{
		auto img = (Image*)it->key;
		total += img->getBuffer().size();
	}

	return total;
}

//-----------------------------------//

NAMESPACE_GRAPHICS_END