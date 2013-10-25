/************************************************************************
*
* Flood Project © (2008-201x)
* Licensed under the simplified BSD license. All rights reserved.
*
************************************************************************/

#include "Engine/API.h"
#include "Engine/Resources/Text.h"

NAMESPACE_ENGINE_BEGIN

//-----------------------------------//

REFLECT_CHILD_CLASS(Text, Resource)
REFLECT_CLASS_END()

REFLECT_CHILD_CLASS(TextLoader, ResourceLoader)
REFLECT_CLASS_END()

//-----------------------------------//

TextLoader::TextLoader()
{
	extensions.pushBack("txt");
	extensions.pushBack("glsl");
}

Resource* TextLoader::prepare(ResourceLoadOptions&)
{
	auto text = Allocate(AllocatorGetHeap(), Text);
	return text;
}

bool TextLoader::decode(ResourceLoadOptions& opts)
{
	auto res = (Text*)opts.resource;
	opts.stream->readString(res->data);

	return true;
}

NAMESPACE_ENGINE_END