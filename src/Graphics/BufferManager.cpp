/************************************************************************
*
* Flood Project � (2008-201x)
* Licensed under the simplified BSD license. All rights reserved.
*
************************************************************************/

#include "Graphics/API.h"
#include "Graphics/BufferManager.h"
#include "Graphics/RenderBackend.h"
#include "Graphics/GeometryBuffer.h"

NAMESPACE_GRAPHICS_BEGIN

//-----------------------------------//

BufferEntry::BufferEntry()
	: vb(nullptr)
	, ib(nullptr)
{
}

//-----------------------------------//

BufferManager::BufferManager()
	: backend(nullptr)
{
}

//-----------------------------------//

BufferManager::~BufferManager()
{
}

//-----------------------------------//

BufferEntry* BufferManager::getBuffer(const GeometryBuffer* gb)
{
	if( !gb ) return nullptr;

	auto entry = buffers.get((uint64)gb, BufferEntry());

	if(!buffers.has((uint64)gb))
	{
		entry.vb = backend->createVertexBuffer();
		entry.vb->setBufferAccess( gb->getBufferAccess() );
		entry.vb->setBufferUsage( gb->getBufferUsage() );
		entry.vb->setGeometryBuffer( gb );

		if( gb->isIndexed() )
		{
			entry.ib = backend->createIndexBuffer();
			entry.ib->setBufferAccess( gb->getBufferAccess() );
			entry.ib->setBufferUsage( gb->getBufferUsage() );
			entry.ib->setGeometryBuffer( gb );
		}

		buffers.set((uint64)gb, entry);
		entry = buffers.get((uint64)gb, entry);
	}

	return &entry;
}

//-----------------------------------//

VertexBufferPtr BufferManager::getVertexBuffer(const GeometryBuffer* gb)
{
	BufferEntry* bufs = getBuffer(gb);
	if (!bufs ) return nullptr;

	return bufs->vb;
}

//-----------------------------------//

IndexBufferPtr BufferManager::getIndexBuffer(const GeometryBuffer* gb)
{
	BufferEntry* bufs = getBuffer(gb);
	if (!bufs ) return nullptr;

	return bufs->ib;
}

//-----------------------------------//

NAMESPACE_GRAPHICS_END