/************************************************************************
*
* vapor3D Engine � (2008-2010)
*
*	<http://www.vapor3d.org>
*
************************************************************************/

#include "Core/API.h"
#include "Core/Memory.h"
#include "Core/Log.h"

#define ALLOCATOR_TRACKING
#define ALLOCATOR_DEFAULT_GROUP "General";

#ifdef PLATFORM_WINDOWS
	#define WIN32_LEAN_AND_MEAN
	#define NOMINMAX
	#include <Windows.h>

	#if defined(ENABLE_MEMORY_LEAK_DETECTOR) && defined(DEBUG_BUILD)
		#include <vld.h>
	#endif
#endif

NAMESPACE_BEGIN

//-----------------------------------//

// Since all memory allocations must go through an allocator,
// there needs to be a default allocator to allocate memory.

static Allocator* GetDefaultHeapAllocator();
static Allocator* GetDefaultStackAllocator();

Allocator* AllocatorGetHeap() { return GetDefaultHeapAllocator(); }
Allocator* AllocatorGetStack() { return GetDefaultStackAllocator(); }

static bool AllocatorSimulateLowMemory = false;

//-----------------------------------//

struct AllocationGroup
{
	AllocationGroup() : freed(0), total(0) { }

	int64 freed;
	int64 total;
};

struct AllocationMetadata
{
	AllocationMetadata()
		: size(0), group(nullptr), allocator(nullptr)
	{ }

	int32 size;
	const char* group;
	Allocator* allocator;
	int32 pattern;
};

const int32 MEMORY_PATTERN = 0xDEADF69F;

typedef std::map<const char*, AllocationGroup, RawStringCompare> MemoryGroupMap;

static MemoryGroupMap& GetMemoryGroupMap()
{
	static MemoryGroupMap memoryGroups;
	return memoryGroups;
}

static void AllocatorTrackGroup(AllocationMetadata* metadata, bool alloc)
{
	if(!metadata) return;

	MemoryGroupMap& memoryGroups = GetMemoryGroupMap();
	memoryGroups[metadata->group].total += alloc ? metadata->size : 0;
	memoryGroups[metadata->group].freed += alloc ? 0 : metadata->size;
}

//-----------------------------------//

void AllocatorDumpInfo()
{
	MemoryGroupMap& memoryGroups = GetMemoryGroupMap();
	if(memoryGroups.empty()) return;

	LogDebug("-----------------------------------------------------");
	LogDebug("Memory stats");
	LogDebug("-----------------------------------------------------");

	MemoryGroupMap::iterator it;
	for(it = memoryGroups.begin(); it != memoryGroups.end(); it++)
	{
		const char* id = it->first;
		AllocationGroup& group = it->second;
		
		const char* fs = "%s\t| total freed: %I64d bytes, total allocated: %I64d bytes";
		String format = StringFormat(fs, id, group.freed, group.total );

		LogDebug( format.c_str() );
	}

	LogDebug("");
}

//-----------------------------------//

void AllocatorDestroy( Allocator* object )
{
	Deallocate<Allocator>(object);
}

//-----------------------------------//

void* AllocatorAllocate( Allocator* alloc, int32 size, int32 align )
{
	return alloc->allocate( alloc, size, align );
}

//-----------------------------------//

void AllocatorDeallocate( const void* object )
{
	if( !object ) return;

	char* addr = (char*) object - sizeof(AllocationMetadata);
	AllocationMetadata* metadata = (AllocationMetadata*) addr;
	Allocator* alloc = metadata->allocator;

	if(metadata->pattern != MEMORY_PATTERN)
	{
		free((void*)object);
		return;
	}

	// Deallocates the memory.
	alloc->deallocate( alloc, object );
}

//-----------------------------------//

static void* HeapAllocate(Allocator* alloc, int32 size, int32 align)
{
	if(AllocatorSimulateLowMemory) return nullptr;
	
	int32 total_size = size + sizeof(AllocationMetadata);
	void* instance = nullptr;
	
#if 0
	if(align == 0)
		instance = malloc(total_size);
	else
		instance = _aligned_malloc(total_size, align);
#endif

	instance = malloc(total_size);

	AllocationMetadata* metadata = (AllocationMetadata*) instance;
	metadata->size = size;
	metadata->group = alloc->group;
	metadata->allocator = alloc;
	metadata->pattern = MEMORY_PATTERN;

#ifdef ALLOCATOR_TRACKING
	AllocatorTrackGroup(metadata, true);
#endif

	return (char*) instance + sizeof(AllocationMetadata);
}

static void HeapDellocate(Allocator* alloc, const void* p)
{
#ifdef ALLOCATOR_TRACKING
	void* base = (char*) p - sizeof(AllocationMetadata);
	AllocatorTrackGroup((AllocationMetadata*) base, false);
#endif

	// _aligned_free
	free(base);
}

//-----------------------------------//

Allocator* AllocatorCreateHeap( Allocator* alloc, const char* group )
{
	Allocator* heap = Allocate(Allocator, alloc);

	heap->allocate = HeapAllocate;
	heap->deallocate = HeapDellocate;
	heap->group = group;

	return heap;
}

//-----------------------------------//

static Allocator* GetDefaultHeapAllocator()
{
	static Allocator heap;
	heap.allocate = HeapAllocate;
	heap.deallocate = HeapDellocate;
	heap.group = ALLOCATOR_DEFAULT_GROUP;
	return &heap;
}

//-----------------------------------//

static void* StackAllocate(Allocator* alloc, int32 size, int32 align)
{
	if(AllocatorSimulateLowMemory) return nullptr;

#ifdef ALLOCATOR_TRACKING
	//AllocatorTrackGroup(alloc, size);
#endif

	return alloca(size);
}

//-----------------------------------//

static void StackDellocate(Allocator* alloc, const void* p)
{
#ifdef ALLOCATOR_TRACKING
	//AllocatorTrackGroup(alloc, 0);
#endif

	// Stack memory is automatically freed.
}

//-----------------------------------//

static Allocator* GetDefaultStackAllocator()
{
	static Allocator stack;
	stack.allocate = StackAllocate;
	stack.deallocate = StackDellocate;
	stack.group = ALLOCATOR_DEFAULT_GROUP;
	return &stack;
}

//-----------------------------------//

static void* PoolAllocate(Allocator* alloc, int32 size, int32 align)
{
	if(AllocatorSimulateLowMemory) return nullptr;

	PoolAllocator* pool = (PoolAllocator*) alloc;

	void* p = pool->current; 
	pool->current += size;

	return p;
}

//-----------------------------------//

static void PoolDellocate(Allocator* alloc, const void* p)
{
}

//-----------------------------------//

Allocator* AllocatorCreatePool( Allocator* alloc,  int32 size )
{
	int32 total = sizeof(PoolAllocator) + size;
	
	PoolAllocator* pool = (PoolAllocator*) alloc->allocate(alloc, total, alignof(PoolAllocator));

	pool->current = (uint8*) pool + sizeof(PoolAllocator);  
	pool->allocate = PoolAllocate;
	pool->deallocate = PoolDellocate;

	return pool;
}

//-----------------------------------//

Allocator* AllocatorCreatePage( Allocator* alloc )
{
	return nullptr;
}

//-----------------------------------//

Allocator* AllocatorCreateTemporary( Allocator* alloc )
{
	return nullptr;
}

//-----------------------------------//

NAMESPACE_END

//-----------------------------------//

// Global new / delete operator overrides.
//using namespace vapor;
//
//void* operator new (size_t size)
//{
//	Allocator* alloc = AllocatorGetHeap();
//	return alloc->allocate(alloc, size, 0);
//}
//
//void operator delete (void *p)
//{
//	AllocatorDeallocate(p);
//}