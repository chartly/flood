/************************************************************************
*
* Flood Project © (2008-201x)
* Licensed under the simplified BSD license. All rights reserved.
*
************************************************************************/

#include "Core/API.h"
#include "Core/Handle.h"
#include "Core/Log.h"

NAMESPACE_CORE_BEGIN

//-----------------------------------//

HandleManager* HandleCreateManager( Allocator* alloc )
{
	HandleManager* man = Allocate(alloc, HandleManager);
	man->nextHandle = 0;
	return man;
}

//-----------------------------------//

void HandleDestroyManager( HandleManager* man )
{
	if( !man ) return;

	const HandleMap& handles = man->handles;

	if( handles.size() > 0 )
	{
		//LogAssert("Handle manager should not have any handles");
		goto out;
	}

out:

	Deallocate(man);
}

//-----------------------------------//

HandleId HandleCreate(HandleManager* man, ReferenceCounted* ref)
{
	if( !man ) return HandleInvalid;
	
	HandleMap& handles = man->handles;
	
	HandleId id = man->nextHandle.increment();
	handles.set(id, ref);
	
	return id;
}

//-----------------------------------//

void HandleDestroy(HandleManager* man, HandleId id)
{
	if( !man ) return;
	man->handles.remove(id);
}

//-----------------------------------//

void HandleGarbageCollect(HandleManager* man)
{
}

//-----------------------------------//

ReferenceCounted* HandleFind(HandleManager* man, HandleId id)
{
	return man->handles.get(id, HandleInvalid);
}

//-----------------------------------//

NAMESPACE_CORE_END