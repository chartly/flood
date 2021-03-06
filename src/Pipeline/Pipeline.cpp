/************************************************************************
*
* Flood Project � (2008-201x)
* Licensed under the simplified BSD license. All rights reserved.
*
************************************************************************/

#include "Pipeline/API.h"
#include "Pipeline/ResourceProcessor.h"
#include "Resources/ResourceLoader.h"
#include "Resources/ResourceManager.h"
#include "Core/Log.h"

#include "Pipeline/ImageProcessor.h"
#include "Pipeline/MeshProcessor.h"

#include "Pipeline/ImporterMilkshape.h"
#include "Pipeline/ImporterFBX.h"

#ifdef API_PIPELINE_DLL_EXPORT
FL_INSTANTIATE_TEMPLATES()
#endif

NAMESPACE_PIPELINE_BEGIN

//-----------------------------------//

static void ReferenceProcessors()
{
	MeshProcessorGetType();
	ImageProcessorGetType();
}

//-----------------------------------//

static void ReferenceImporters()
{
#ifdef ENABLE_IMPORTER_MILKSHAPE
	ImporterMilkshapeGetType();
#endif

#ifdef ENABLE_IMPORTER_FBX
	ImporterFBXGetType();
#endif
}

//-----------------------------------//

void PipelineInit()
{
	ReferenceProcessors();
	ReferenceImporters();

	Class* klass = ResourceProcessorGetType();
	
	for( size_t i = 0; i < klass->childs.size(); i++ )
	{
		Class* child = klass->childs[i];
		
		ResourceProcessor* processor = (ResourceProcessor*) ClassCreateInstance(child, AllocatorGetHeap());
		resourceProcessors.pushBack(processor);

		LogInfo("Registering asset handler: %s", child->name);
	}
}

//-----------------------------------//

void PipelineCleanup()
{
	for( size_t i = 0; i < resourceProcessors.size(); i++ )
	{
		ResourceProcessor* processor = resourceProcessors[i];
		Deallocate(processor);
	}

	resourceProcessors.clear();
}

//-----------------------------------//

ResourceProcessor* PipelineFindProcessor(Class* type)
{
	for( size_t i = 0; i < resourceProcessors.size(); i++ )
	{
		ResourceProcessor* processor = resourceProcessors[i];
		
		bool isProcessor = ClassInherits(type, processor->GetResourceType());
		if( isProcessor ) return processor;
	}

	return nullptr;
}

//-----------------------------------//

#ifdef BUILDING_STANDALONE

#define INSTANTIATE_TEMPLATES

ResourceManager* resources;

//-----------------------------------//

void showResourceLoaders()
{
	printf("\nKnown resource loaders:\n\n");

	Class& type = ResourceLoader::getStaticType();
	
	for( size_t i = 0; i < type.childs.size(); i++ )
	{
		Class& child = *type.childs[i];
		printf("\t%s\n", child.name.c_str());
	} 
}

//-----------------------------------//

void showResourceProcessors()
{
	printf("\nKnown resource processors:\n\n");

	Class& type = ResourceProcessor::getStaticType();
	
	for( size_t i = 0; i < type.childs.size(); i++ )
	{
		Class& child = *type.childs[i];
		printf("\t%s\n", child.name.c_str());
	}
}

//-----------------------------------//

int main(int argc, char* argv[])
{
	printf("Syntax: Pipeline.exe -in res.foo -out res.bar\n");

	resources = new ResourceManager();

	showResourceLoaders();
	showResourceProcessors();

	return 0;
}

//-----------------------------------//

#endif

NAMESPACE_PIPELINE_END