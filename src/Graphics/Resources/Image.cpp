/************************************************************************
*
* Flood Project � (2008-201x)
* Licensed under the simplified BSD license. All rights reserved.
*
************************************************************************/

#include "Graphics/API.h"
#include "Graphics/Resources/Image.h"
#include "Core/Log.h"
#include "Core/Stream.h"
#include "FreeImage.h"

//#define ENABLE_IMAGE_WRITER
//#define ENABLE_IMAGE_LODEPNG

#ifdef ENABLE_IMAGE_LODEPNG
	#include "lodepng.h"
#endif

NAMESPACE_GRAPHICS_BEGIN

//-----------------------------------//

REFLECT_ENUM(PixelFormat)
	ENUM(R8G8B8A8)
	ENUM(R8G8B8)
	ENUM(Depth)
	ENUM(DXT1)
	ENUM(DXT1a)
	ENUM(DXT3)
	ENUM(DXT5)
	ENUM(DXT5nm)
	ENUM(Unknown)
REFLECT_ENUM_END()
 
REFLECT_CHILD_CLASS(Image, Resource)
	FIELD_ENUM(4, PixelFormat, format) FIELD_READONLY(format)
REFLECT_CLASS_END()

//-----------------------------------//

ImageHandle ImageCreate(Allocator* alloc, uint32 width, uint32 height, PixelFormat format)
{
	Image* image = Allocate(alloc, Image);
	image->create(width, height, format);

	return HandleCast<Image>( ResourceHandleCreate(image) );
}

//-----------------------------------//

Image::Image()
	: width(0)
	, height(0)
	, format(PixelFormat::Unknown)
    , timestamp(1)
{ }

//-----------------------------------//

Image::Image(uint32 _width, uint32 _height, PixelFormat _format)
	: width(0)
	, height(0)
	, format(PixelFormat::Unknown)
    , timestamp(1)
{
	create(_width, _height, _format);
}

//-----------------------------------//

uint32 Image::getPixelSize()
{
    switch(format)
    {
    case PixelFormat::R8G8B8A8:
    case PixelFormat::B8G8R8A8:
        return 4;
    case PixelFormat::R8G8B8:
    case PixelFormat::B8G8R8:
        return 3;
    case PixelFormat::Depth:
        return 1;
    default:
        assert(false);
    }

    return 0;
}

//-----------------------------------//

uint32 Image::getSize()
{
    return width*height*getPixelSize();
}

//-----------------------------------//

void Image::create(uint32 _width, uint32 _height, PixelFormat _format)
{
	this->width  = _width;
	this->height = _height;
	this->format = _format;

    buffer.resize(getSize());
}

//-----------------------------------//

void Image::setBuffer(byte* data)
{
    uint32 size = getSize();

    if(buffer.size() != size)
        buffer.resize(size);

    memcpy(buffer.data(), data, size * sizeof(byte));

    timestamp++;
}

//-----------------------------------//

void Image::setBuffer(byte* data, uint stride)
{
    assert(stride >= width*getPixelSize());

    uint32 size = getSize();

    if(buffer.size() != size)
        buffer.resize(size);

    for(int i=0; i<height; ++i)
        memcpy( buffer.data() + i*width*getPixelSize(), 
                data + i*stride,
                width*getPixelSize()*sizeof(byte));

    timestamp++;
}

//-----------------------------------//

void Image::setBuffer(Image* image, Vector2i offset)
{
    assert(offset.x + image->width <= width);
    assert(offset.y + image->height <= height);
    assert(image->format == this->format);

    uint32 expectedSize = getSize();
    if (buffer.size() != expectedSize)
        buffer.resize(expectedSize);

    uint32 pixelSize = getPixelSize();
    auto destData = buffer.data() +  (offset.x + offset.y * width) * pixelSize;
    auto origData = image->buffer.data();
    for (int i = 0; i < image->height; ++i)
    {
        memcpy(destData, origData, image->width * pixelSize * sizeof(byte));
        destData += width * pixelSize;
        origData += image->width * pixelSize;
    }

    timestamp++;
}

//-----------------------------------//

bool Image::isCompressed() const
{
	switch(format)
	{
	case PixelFormat::DXT1:
	case PixelFormat::DXT1a:
	case PixelFormat::DXT3:
	case PixelFormat::DXT5:
	case PixelFormat::DXT5nm:
		return true;
	}

	return false;
}

//-----------------------------------//

void Image::setColor( const Color& color )
{
	if( format != PixelFormat::R8G8B8A8 )
		return;

	for( size_t i = 0; i < buffer.size(); i += 4 )
	{
		buffer[i+0] = byte(color.r * 255);
		buffer[i+1] = byte(color.g * 255);
		buffer[i+2] = byte(color.b * 255);
		buffer[i+3] = 1;
	}

    timestamp++;
}

void Image::SetModified()
{
    timestamp++;
}

//-----------------------------------//

void Image::log() const
{
	const char* desc = EnumGetValueName(PixelFormatGetType(), (int32)format);
	LogInfo( "Image has pixel format '%s' and size %dx%d", desc, width, height );
}

//-----------------------------------//

void ImageWriter::save( Image* image, Stream* stream )
{
#ifdef ENABLE_IMAGE_WRITER
	if( !stream ) return;

	// TODO: sleep until the image is not loaded
	if( !image->isLoaded() ) return;

	if( image->isCompressed() )
		output = image->getBuffer();
	else if( !convert(image) )
		return;

	if( output.empty() ) return;

	StreamWrite(stream, output.data(), output.size());
#endif
}

//-----------------------------------//

void ImageWriter::save( Image* image, const char* filePath )
{
    if(image->getPixelFormat() != PixelFormat::B8G8R8A8)
    {
        LogError("Image to be saved must be of format BGRA.");
        return;
    }

    FIBITMAP* fiBitmap = FreeImage_Allocate(image->getWidth(), image->getHeight(), image->getPixelSize()*8);

    BYTE* data = FreeImage_GetBits(fiBitmap);

	memcpy(data, image->getBuffer().data(), image->getSize());

	auto fileFormat = FreeImage_GetFIFFromFilename(filePath);
	if (fileFormat == FIF_UNKNOWN)
	{
		LogError("Could not save image of unknown format.");
	}
	else
	{
		if (!FreeImage_Save(fileFormat, fiBitmap, filePath))
			LogError("Image could not be saved.");
	}

    FreeImage_Unload(fiBitmap);
}


//-----------------------------------//

bool ImageWriter::convert( Image* image )
{
    #pragma TODO("Fix the LodePNG image writing code to the new API")

#if defined(ENABLE_IMAGE_WRITER_)
	LodePNG::Encoder encoder;
	
	LodePNG_InfoColor& png = encoder.getInfoPng().color;
	LodePNG_InfoColor& raw = encoder.getInfoRaw().color;
	
	switch(image->getPixelFormat())
	{
	case PixelFormat::Depth:
		png.colorType = 0;
		raw.colorType = 0;
		break;
	case PixelFormat::R8G8B8:
		png.colorType = 2;
		raw.colorType = 2;
		break;
	case PixelFormat::R8G8B8A8:
		png.colorType = 6;
		raw.colorType = 6;
		break;
	default:
		LogError("Pixel format is not supported");
		return false;
	}

	raw.bitDepth = 8;
	
	encoder.encode(output, image->getBuffer(),
		image->getWidth(), image->getHeight());

	if( encoder.hasError() )
	{
		LogError("Could not encode image to PNG");
		return false;
	}

	return true;
#else
	return false;
#endif
}

//-----------------------------------//

NAMESPACE_GRAPHICS_END