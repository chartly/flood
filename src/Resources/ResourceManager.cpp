/************************************************************************
*
* Flood Project © (2008-201x)
* Licensed under the simplified BSD license. All rights reserved.
*
************************************************************************/

#include "Resources/API.h"
#include "Resources/ResourceManager.h"
#include "Resources/ResourceLoader.h"

#include "Core/Log.h"
#include "Core/Memory.h"
#include "Core/Concurrency.h"
#include "Core/Stream.h"
#include "Core/Archive.h"
#include "Core/Utilities.h"
#include "Core/Serialization.h"
#include "Core/Math/Hash.h"

NAMESPACE_RESOURCES_BEGIN

//-----------------------------------//

static Allocator* gs_ResourcesAllocator = nullptr;
Allocator* GetResourcesAllocator() { return gs_ResourcesAllocator; }

void ResourcesInitialize()
{
	gs_ResourcesAllocator = AllocatorCreateHeap( AllocatorGetHeap() );
	AllocatorSetGroup(gs_ResourcesAllocator, "Resources");
}

void ResourcesDeinitialize()
{
	AllocatorDestroy(gs_ResourcesAllocator);
}

//-----------------------------------//

static HandleManager* gs_ResourceHandleManager = nullptr;

ReferenceCounted* ResourceHandleFind(HandleId id)
{
	if( !gs_ResourceHandleManager ) return nullptr;

	Resource* res = (Resource*) HandleFind(gs_ResourceHandleManager, id);
	return res;
}

static const char* GetResourceGroupString(ResourceGroup group)
{
	switch(group)
	{
	case ResourceGroup::General: return "General";
	case ResourceGroup::Images: return "Images";
	case ResourceGroup::Meshes: return "Meshes";
	case ResourceGroup::Fonts: return "Fonts";
	case ResourceGroup::Shaders: return "Shaders";
	case ResourceGroup::Audio: return "Audio";
	case ResourceGroup::Scripts: return "Scripts";
	case ResourceGroup::Scenes: return "Scenes";
	case ResourceGroup::Materials: return "Materials";
	case ResourceGroup::Particles: return "Particles";
	}

	assert(0 && "Unreachable");
	return nullptr;
}

ResourceHandle ResourceHandleCreate(Resource* res)
{
	if( !gs_ResourceHandleManager ) return HandleInvalid;
	
	HandleId handle = HandleCreate(gs_ResourceHandleManager, res);
	LogDebug("ResourceHandleCreate: %s %lu %s",
		GetResourceGroupString(res->getResourceGroup()), handle,
		res->getPath().c_str());
	
	return handle;
}

static bool gs_RemoveResource = true;

void ResourceHandleDestroy(HandleId id)
{
	Resource* resource = (Resource*) ResourceHandleFind(id);

	if( gs_RemoveResource )
		fldCore()->resourceManager->removeResource(resource);
	
	LogDebug("ResourceHandleDestroy: %lu", id);
	HandleDestroy(gs_ResourceHandleManager, id);
}

static HandleId ResourceHandleFind(const char* s)
{
	if( !fldCore()->resourceManager ) return HandleInvalid;
	return fldCore()->resourceManager->loadResource(s).getId();
}

static void ResourceHandleSerialize(
	ReflectionContext* context, ReflectionWalkType wt )
{
	Serializer* serializer = (Serializer*) context->userData;

	Resource* resource = (Resource*) context->object;
	
	context->valueContext.s = &resource->path;
	context->primitive = &PrimitiveGetBuiltins().p_string;
	context->walkPrimitive(context, wt);
}

//-----------------------------------//

ResourceEvent::ResourceEvent()
	: resource(nullptr)
	, oldResource(nullptr)
{
}

//-----------------------------------//

ResourceManager::ResourceManager()
	: taskPool(nullptr)
	, archive(nullptr)
	, handleManager(nullptr)
	, numResourcesQueuedLoad(0)
	, asynchronousLoading(true)
{
	handleManager = HandleCreateManager( GetResourcesAllocator() );

	assert(gs_ResourceHandleManager == nullptr);
	gs_ResourceHandleManager = handleManager;

	ReflectionHandleContext context;
	context.type = ReflectionGetType(Resource);
	context.handles = gs_ResourceHandleManager;
	context.serialize = ResourceHandleSerialize;
	context.deserialize = ResourceHandleFind;
	
	ReflectionHandleContextMap contextMap;
	ReflectionSetHandleContext(&contextMap, context);

	resourceFinishLoadMutex = Allocate(GetResourcesAllocator(), Mutex);
	resourceFinishLoad = Allocate(GetResourcesAllocator(), Condition);
}

//-----------------------------------//

ResourceManager::~ResourceManager()
{
	destroyHandles();
	resourceLoaders.clear();

	Deallocate(resourceFinishLoad);
	Deallocate(resourceFinishLoadMutex);
}

//-----------------------------------//

void ResourceManager::destroyHandles()
{
	for( auto it = resources.begin(); it != resources.end(); ++it )
	{
		auto& handle = it->value;
		Resource* res = handle.Resolve();
		
		LogDebug("Resource %s (refs: %d)", res->getPath().c_str(),
			res->references.read());
	}

	gs_RemoveResource = false;
	resources.clear();
	
	HandleDestroyManager(handleManager);
	handleManager = nullptr;
}

//-----------------------------------//

ResourceHandle ResourceManager::getResource(const String& path)
{
	Path name = PathGetFile(path);
	auto key = MurmurHash64(name.c_str(), name.size(), 0);

	return resources.get(key, HandleInvalid);
}

//-----------------------------------//

ResourceHandle ResourceManager::loadResource(const String& path)
{
	Path name = PathGetFile(path);

	ResourceLoadOptions options;
	options.name = name;
	options.asynchronousLoad = asynchronousLoading;
	
	return loadResource(options);
}

//-----------------------------------//

ResourceHandle ResourceManager::loadResource(ResourceLoadOptions& options)
{
	if( !archive ) return ResourceHandle(HandleInvalid);

	Path fileExt = PathGetFileExtension(options.name);
	
	// If the file has no extension, search for one with the same
	// name but with known resource loadable file extensions.

	if(fileExt.empty() && !findResource(options))
	{
		LogError("Could not find matching file for '%s'", options.name.c_str());
		return ResourceHandle(HandleInvalid);
	}

	// Check if the resource is already loaded.
	ResourceHandle handle = getResource(options.name);
	if( handle ) return handle;

	if( !validateResource(options.name) )
		return ResourceHandle(HandleInvalid);

	Resource* resource = prepareResource(options);
	
	if( !resource )
		return ResourceHandle(HandleInvalid); 

	handle = ResourceHandleCreate(resource);
	
	if(handle == HandleInvalid)
		return ResourceHandle(HandleInvalid);

	// Register the decoded resource in the map.
	Path base = PathGetFile(options.name);
	auto key = MurmurHash64(base.c_str(), base.size(), 0);
	resources.set(key, handle);

	decodeResource(options);

	return handle;
}

//-----------------------------------//

bool ResourceManager::findResource(ResourceLoadOptions& options)
{	
	for(auto it = resourceLoaders.begin(); it != resourceLoaders.end(); it++)
	{
		auto loader = it->value.get();
		for(auto ext : loader->extensions)
		{
			auto newPath = StringFormat("%s.%s", options.name.c_str(), ext.c_str());

			if (archive->existsFile(newPath))
			{
				options.name = PathNormalize(newPath);
				options.loader = loader;
				return true;
			}
		}
	}

	return false;
}

//-----------------------------------//

bool ResourceManager::validateResource( const Path& path )
{
	if( path.empty() ) 
		return false;
	
	auto extension = PathGetFileExtension(path);
	if( extension.empty() )
	{
		LogWarn( "Resource '%s' has an invalid extension", path.c_str() );
		return false;
	}

	if( !findLoader(extension) )
	{
		LogWarn("No resource loader found for resource '%s'", path.c_str());
		return false;
	}

	return true;
}

//-----------------------------------//

Resource* ResourceManager::prepareResource( ResourceLoadOptions& options )
{
	auto stream = archive->openFile(options.name, GetResourcesAllocator());
	if( !stream )
	{
		LogWarn("Resource was not found: '%s'", options.name.c_str());
		return nullptr;
	}

	auto file = PathGetFile(options.name);
	auto loader = options.loader != nullptr ? options.loader : findLoader( PathGetFileExtension(file) );
	if( !loader )
	{
		LogWarn("No resource loader found for resource '%s'", file.c_str());
		return nullptr;
	}

	options.loader = loader;
	options.stream = stream;

	auto resource = loader->prepare(options);
	if( !resource )
	{
		LogError("Error preparing resource: '%s'", options.name.c_str());
		return nullptr;
	}

	resource->setStatus( ResourceStatus::Loading );
	resource->setPath( file );

	options.resource = resource;

	return resource;
}

//-----------------------------------//

void ResourceTaskRun(Task* task);

void ResourceManager::decodeResource( ResourceLoadOptions& options )
{
	auto task = Allocate(GetResourcesAllocator(), Task);
	auto taskOptions = Allocate(GetResourcesAllocator(), ResourceLoadOptions);

	*taskOptions = options;

	task->callback.Bind(ResourceTaskRun);
	task->userdata = taskOptions;

	numResourcesQueuedLoad.increment();

#ifdef ENABLE_THREADED_LOADING
	if( taskPool && asynchronousLoading && options.asynchronousLoad )
	{
		taskPool->add(task, options.isHighPriority);
		return;
	}
#endif

	task->run();
	sendPendingEvents();
}

//-----------------------------------//

void ResourceManager::loadQueuedResources()
{
	resourceFinishLoadMutex->lock();

	while(numResourcesQueuedLoad.read() > 0 )
	{
		#pragma TODO("Use a sleep and notify the observers of progress")
		resourceFinishLoad->wait(*resourceFinishLoadMutex);
	}

	resourceFinishLoadMutex->unlock();

	assert(numResourcesQueuedLoad.read() == 0);
}

//-----------------------------------//

void ResourceManager::update()
{
	sendPendingEvents();
	removeUnusedResources();
}

//-----------------------------------//

void ResourceManager::sendPendingEvents()
{
	ResourceEvent event;

	while( resourceEvents.try_pop_front(event) )
	{
		Resource* resource = event.resource;
		auto base = PathGetFile(resource->path);
		auto key = MurmurHash64(base.c_str(), base.size(), 0);
		ResourceHandle handle = resources.get(key, HandleInvalid);
		assert( handle != HandleInvalid );

		event.handle = handle;
		onResourceLoaded( event );
	}
}

//-----------------------------------//

void ResourceManager::removeUnusedResources()
{
	#pragma TODO("Finish the unused resource collecetor")
	return;

	Array<uint64> resourcesToRemove;
	for(auto e : resources)
	{
		auto resource = e.value;
		if( resource.Resolve()->references.read() == 1 )
			resourcesToRemove.pushBack(e.key);
	}

	for(auto key : resourcesToRemove)
		removeResource(key);
}

//-----------------------------------//

void ResourceManager::removeResource(Resource* resource)
{
	if( !resource ) return;

	const String& path = resource->getPath();
	removeResource(path);
}



void ResourceManager::removeResource(const String& path)
{
	auto key = MurmurHash64(path.c_str(), path.size(), 0);
	removeResource(key);

	LogInfo("Unloaded resource: %s", path.c_str());
}

//-----------------------------------//

void ResourceManager::removeResource(uint64 key)
{
	auto handle = resources.get(key, HandleInvalid);
	if(handle == HandleInvalid)
		return;

	ResourceEvent event;
	event.handle = handle;
	onResourceRemoved( event );

	resources.remove(key);
}

//-----------------------------------//

void ResourceManager::registerLoader(ResourceLoader* loader)
{
	if( !loader ) return;

	Class* klass = loader->getType();
	LogInfo( "Registering resource loader '%s'", klass->name );
	
	for( auto ext : loader->getExtensions() )
	{
		auto key = MurmurHash64(ext.c_str(), ext.size(), 0);
		if(resourceLoaders.has(key))
		{
			LogDebug("Extension '%s' already has a resource loader", ext.c_str());
			continue;
		}

		resourceLoaders.set(key, loader);
	}

	onResourceLoaderRegistered( *loader );
}

//-----------------------------------//

ResourceLoader* ResourceManager::findLoader(const String& extension)
{
	auto ext = StringToLowerCase(extension);
	auto key = MurmurHash64(ext.c_str(), ext.size(), 0);

	return resourceLoaders.get(key, nullptr).get();
}

//-----------------------------------//

ResourceLoader* ResourceManager::findLoaderByClass(const Class* klass)
{
	for(auto e : resourceLoaders)
	{
		auto& loader = e.value;
		auto resourceClass = loader->getResourceClass();
		
		if(ClassInherits(resourceClass, klass))
			return loader.get();
	}

	return nullptr;
}

//-----------------------------------//

void ResourceManager::setupResourceLoaders(Class* klass)
{
	for( size_t i = 0; i < klass->childs.size(); i++ )
	{
		Class* child = klass->childs[i];

		if( !child->childs.empty() )
			setupResourceLoaders(child);
	
		if( ClassIsAbstract(child ) ) continue;

		auto loader = (ResourceLoader*) ClassCreateInstance(
			child, GetResourcesAllocator());

		registerLoader( loader );
	}
}

//-----------------------------------//

void ResourceManager::setArchive(Archive* newArchive)
{
	if(archive == newArchive) return;

	if(archive)
	{
		// Disconnect from the watch events.
		archive->onWatchEvent.Disconnect(this, &ResourceManager::handleWatchResource);
		archive = nullptr;
	}

	if(newArchive)
	{
		archive = newArchive;
		archive->onWatchEvent.Connect(this, &ResourceManager::handleWatchResource);
	}
}

//-----------------------------------//

void ResourceManager::handleWatchResource(Archive*, const FileWatchEvent& evt)
{
	// Check if the filename maps to a known resource.
	auto file = PathGetFile(evt.filename);
	auto key = MurmurHash64(file.c_str(), file.size(), 0);

	if( !resources.has(key) )
		return; // Resource is not known.

	// Reload the resource if it was modified.
	
	if( evt.action == FileWatchEventKind::Deleted )
	{
#pragma TODO("Add deletion support in live updating")

		String msg;
		mini::format(msg, "> Resource \"%0\" was deleted. -- not implemented.", file.c_str());
		LogDebug(msg.c_str());

		return;
	}
	if( evt.action == FileWatchEventKind::Renamed )
	{
		String msg;
		mini::format(msg, "Resource \"%0\" is being swapped out with a new version.", file.c_str());
		LogDebug(msg.c_str());
	}

	// Register the decoded resource in the map.
	LogInfo("Reloading resource '%s'", file.c_str());

	ResourceLoadOptions options;
	options.sendLoadEvent = false;
	options.name = evt.filename;
	
	auto resource = prepareResource(options);
	decodeResource(options);

	auto handle = resources.get(key, HandleInvalid);
	auto handleId = handle.getId();
	auto oldResource = handle.Resolve();

	ResourceEvent event;
	event.resource = resource;
	event.oldResource = oldResource;
	event.handle = handle;

	handleManager->handles.set(handleId, resource);

	onResourceReloaded(event);

	event.handle.id = HandleInvalid;
}

//-----------------------------------//

NAMESPACE_RESOURCES_END