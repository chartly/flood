/************************************************************************
*
* Flood Project � (2008-201x)
* Licensed under the simplified BSD license. All rights reserved.
*
************************************************************************/

#include "Resources/API.h"
#include "Resources/ResourceLoader.h"
#include "Resources/ResourceManager.h"

NAMESPACE_RESOURCES_BEGIN

//-----------------------------------//

REFLECT_ABSTRACT_CHILD_CLASS(ResourceLoader, Object)
	//FIELD_VECTOR(string, extensions) FIELD_READONLY(extensions)
REFLECT_CLASS_END()

//-----------------------------------//

ResourceStream::ResourceStream()
	: stream(nullptr)
	, loader(nullptr)
{

}

//-----------------------------------//

ResourceStream::~ResourceStream()
{
	Deallocate(stream);
}

//-----------------------------------//

ResourceLoadOptions::ResourceLoadOptions()
	: group(ResourceGroup::General)
	, stream(nullptr)
	, loader(nullptr)
	, resource(nullptr)
	, asynchronousLoad(true)
	, sendLoadEvent(true)
	, isHighPriority(false)
	, keepStreamOpen(false)
{
}

//-----------------------------------//

void ResourceLoadOptions::addOption(int key, int value)
{
	option.key = key;
	option.value = value;
}

//-----------------------------------//

NAMESPACE_RESOURCES_END