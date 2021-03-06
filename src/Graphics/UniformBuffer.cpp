/************************************************************************
*
* Flood Project � (2008-201x)
* Licensed under the simplified BSD license. All rights reserved.
*
************************************************************************/

#include "Graphics/API.h"

#ifdef ENABLE_RENDERER_OPENGL

#include "Graphics/Graphics.h"
#include "Graphics/UniformBuffer.h"
#include "Core/Math/Matrix4x3.h"
#include "Core/Math/Matrix4x4.h"
#include "Core/Math/Hash.h"

NAMESPACE_GRAPHICS_BEGIN

//-----------------------------------//

static char* AllocateName(const char* name)
{
	if( !name ) return nullptr;

	size_t nameSize = sizeof(char) * strlen(name);
	char* newName = (char*) AllocatorAllocate(GetFrameAllocator(), nameSize+1, 0);
	memcpy(newName, name, nameSize);
	newName[nameSize] = '\0';
	return newName;
}

//-----------------------------------//

UniformBufferElement* UniformBuffer::getElement(const char* name, size_t size)
{
	//UniformBufferElements::iterator it = elements.find( name.c_str() );
	//if( it != elements.end() ) return it->second;

	void* p = AllocatorAllocate(GetFrameAllocator(), sizeof(UniformBufferElement)+size, 0);
	UniformBufferElement* element = (UniformBufferElement*) p;

	if( !name ) return nullptr;

#ifdef BUILD_DEBUG
	if( strlen(name) == 0 )
	{
		LogAssert("Uniform elements should be named");
		return nullptr;
	}
#endif

	if( !element ) return nullptr;

	element->type = (UniformDataType) 0;
	element->count = 0;
	element->name = (char*) name; // AllocateName(name);

	auto key = MurmurHash64(name, strlen(name), 0);
	elements.set(key, element);
	return element;
}

//-----------------------------------//

void UniformBuffer::removeUniform( const char* slot )
{
	auto key = MurmurHash64(slot, strlen(slot), 0);
	elements.remove(key);
}

//-----------------------------------//

void UniformBuffer::setUniform( const char* name, int32 data )
{
	size_t size = sizeof(int32);
	UniformBufferElement* element = getElement(name, size);
	if( !element ) return;
	element->type = UniformDataType::Scalar_I;
	element->count = 1;
	memcpy(&element->data, &data, size);
}

//-----------------------------------//

void UniformBuffer::setUniform( const char* name, float data )
{
	size_t size = sizeof(float);
	UniformBufferElement* element = getElement(name, size);
	if( !element ) return;
	element->type = UniformDataType::Scalar_F;
	element->count = 1;
	memcpy(&element->data, &data, size);
}

//-----------------------------------//

void UniformBuffer::setUniform( const char* name, const Array<Vector3>& vec )
{
	size_t size = sizeof(Vector3)*vec.size();
	UniformBufferElement* element = getElement(name, size);
	if( !element ) return;
	element->type = UniformDataType::Vector3_F;
	element->count = vec.size();
	memcpy(&element->data, &vec.front(), size);
}

//-----------------------------------//

void UniformBuffer::setUniform( const char* name, const Array<Color>& vec )
{
	assert(0 && "Not implemented yet");

#if 0
	size_t size = sizeof(Color)*vec.size();
	UniformBufferElement* element = getElement(name, size);
	if( !element ) return;
	element->type = UniformDataType::Vector3_F;
	element->count = vec.size();
	memcpy(&element->data, &vec.front(), size);
#endif
}

//-----------------------------------//

void UniformBuffer::setUniform( const char* name, const Vector3& vec )
{
	size_t size = sizeof(Vector3);
	UniformBufferElement* element = getElement(name, size);
	if( !element ) return;
	element->type = UniformDataType::Vector3_F;
	element->count = 1;
	memcpy(&element->data, &vec.x, size);
}

//-----------------------------------//

void UniformBuffer::setUniform( const char* name, const Matrix4x3& matrix )
{
	Matrix4x4 m(matrix);
	size_t size = sizeof(Matrix4x4);
	UniformBufferElement* element = getElement(name, size);
	if( !element ) return;
	element->type = UniformDataType::Matrix4_F;
	element->count = 1;
	memcpy(&element->data, &m.m11, size);
}

//-----------------------------------//

void UniformBuffer::setUniform( const char* name, const Matrix4x4& matrix )
{
	size_t size = sizeof(Matrix4x4);
	UniformBufferElement* element = getElement(name, size);
	if( !element ) return;
	element->type = UniformDataType::Matrix4_F;
	element->count = 1;
	memcpy(&element->data, &matrix.m11, size);
}

//-----------------------------------//

void UniformBuffer::setUniform( const char* name, const Array<Matrix4x4>& vec )
{
	size_t size = sizeof(Matrix4x4)*vec.size();
	UniformBufferElement* element = getElement(name, size);
	if( !element ) return;
	element->type = UniformDataType::Matrix4_F;
	element->count = vec.size();
	memcpy(&element->data, &vec.front(), size);
}

//-----------------------------------//

NAMESPACE_GRAPHICS_END

#endif