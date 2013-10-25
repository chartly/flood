/************************************************************************
*
* Flood Project © (2008-201x)
* Licensed under the simplified BSD license. All rights reserved.
*
************************************************************************/

#pragma once

#include "Resources/ResourceLoader.h"

NAMESPACE_ENGINE_BEGIN

//-----------------------------------//

/**
 * Textual content resource that can be used as a base for all text-based
 * resource types, like scripts, shaders, materials, scenes, etc.
 */

API_ENGINE REFLECT_DECLARE_CLASS(Text)

class API_ENGINE Text : public Resource
{
	REFLECT_DECLARE_OBJECT(Text)

public:
	GETTER(ResourceGroup, ResourceGroup, ResourceGroup::General)

	String data;
};

//-----------------------------------//

API_ENGINE REFLECT_DECLARE_CLASS(TextLoader)
	
class API_ENGINE TextLoader : public ResourceLoader
{
    REFLECT_DECLARE_OBJECT(ResourceLoader)

public:
    TextLoader();

    RESOURCE_LOADER_CLASS(Text)
    GETTER(Name, const String, "text")
    GETTER(ResourceGroup, ResourceGroup, ResourceGroup::General)

    virtual Resource* prepare(ResourceLoadOptions& opts) override;
    virtual bool decode(ResourceLoadOptions& opts) override;
};

TYPEDEF_RESOURCE_HANDLE_FROM_TYPE( Text );

//-----------------------------------//

NAMESPACE_ENGINE_END

//#endif