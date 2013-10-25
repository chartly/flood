/************************************************************************
*
* Flood Project © (2008-201x)
* Licensed under the simplified BSD license. All rights reserved.
*
************************************************************************/

#include "Core/API.h"
#include "Core/Archive.h"
#include "Core/Stream.h"
#include "Core/Memory.h"
#include "Core/Log.h"

#ifdef ENABLE_ARCHIVE_VIRTUAL

NAMESPACE_CORE_BEGIN

//-----------------------------------//

ArchiveVirtual::ArchiveVirtual() 
	: Archive("./")
{
}

//-----------------------------------//

ArchiveVirtual::~ArchiveVirtual()
{
	close();
}

//-----------------------------------//

bool ArchiveVirtual::mount(Archive * mount, const Path& mountPath)
{
	mounts.pushBack(mount);

	// Setup archive watch callbacks.
	mount->userdata = this;
	mount->onWatchEvent.Connect(this, &ArchiveVirtual::onWatchEvent);

	return true;
}

//-----------------------------------//

void ArchiveVirtual::mountDirectories(const Path& dirPath, Allocator* alloc)
{
	Archive* dir = Allocate(alloc, ArchiveDirectory, dirPath);
	if (!dir) return;

	mount(dir, "./");
	
	Array<Path> dirs;
	dir->enumerateDirs(dirs);

	for(auto& dir : dirs)
	{
		auto& path = PathCombine(dirPath, dir);
		Archive* ndir = Allocate(alloc, ArchiveDirectory, path);
		mount(ndir, "./");
	}
}

//-----------------------------------//

bool ArchiveVirtual::open(const Path& path)
{
	isValid = true; 
	return isValid;
}

//-----------------------------------//

bool ArchiveVirtual::close()
{
	for(auto& i : mounts)
	{
		Archive* marchive = i;
		Deallocate(marchive);
	}

	mounts.clear();

	return true;
}

//-----------------------------------//

Stream* ArchiveVirtual::openFile(const Path& path, Allocator* alloc)
{
	if (mounts.empty()) return nullptr;

	Stream* stream = nullptr;

	for(auto& i : mounts)
	{
		Archive* marchive = i;
		stream = marchive->openFile(path, alloc);
		if (stream) break;
	}

	return stream;
}

//-----------------------------------//

void ArchiveVirtual::enumerate(Array<Path>& paths, bool dir)
{

	for(auto& i : mounts)
	{
		Archive* marchive = i;

		if (dir) marchive->enumerateDirs(paths);
		else marchive->enumerateFiles(paths);
	}
}

//-----------------------------------//

void ArchiveVirtual::enumerateFiles(Array<Path>& paths)
{
	enumerate(paths, false);
}

//-----------------------------------//

void ArchiveVirtual::enumerateDirs(Array<Path>& paths)
{
	enumerate(paths, true);
}

//-----------------------------------//

bool ArchiveVirtual::existsFile(const Path& path)
{
	for(auto& i : mounts)
		if (i->existsFile(path))
			return true;

	return false;
}

//-----------------------------------//

bool ArchiveVirtual::existsDir(const Path& path)
{
	for(auto& i : mounts)
		if(i->existsDir(path))
			return true;

	return false;
}

//-----------------------------------//

void ArchiveVirtual::onWatchEvent(Archive* archive, const FileWatchEvent& event)
{
	auto varchive = (ArchiveVirtual*) archive->userdata;
	assert(varchive == this);

	varchive->onWatchEvent(this, event);
}

//-----------------------------------//

NAMESPACE_CORE_END

#endif
