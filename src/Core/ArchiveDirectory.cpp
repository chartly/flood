/************************************************************************
*
* Flood Project © (2008-201x)
* Licensed under the simplified BSD license. All rights reserved.
*
************************************************************************/

#include "Core/API.h"

#ifdef ENABLE_ARCHIVE_DIR

#include "Core/Archive.h"
#include "Core/Stream.h"
#include "Core/Memory.h"
#include "Core/Log.h"
#include "Core/Utilities.h"

NAMESPACE_CORE_BEGIN

//-----------------------------------//

ArchiveDirectory::ArchiveDirectory(const Path& path)
	: Archive(path)
{
	open(path);
}

//-----------------------------------//

ArchiveDirectory::~ArchiveDirectory()
{
	close();
}

//-----------------------------------//

bool ArchiveDirectory::open(const Path& path)
{
	watchId = fldCore()->fileWatcher->addWatch(path, this);
	fldCore()->fileWatcher->onFileWatchEvent.Connect(this, &ArchiveDirectory::onFileWatchEvent);

	isValid = true;
	return isValid;
}

//-----------------------------------//

bool ArchiveDirectory::close()
{
	if (watchId != 0)
	{
		// Remove the archive from the watch list.
		fldCore()->fileWatcher->removeWatch(watchId);
	}

	fldCore()->fileWatcher->onFileWatchEvent.Disconnect(this, &ArchiveDirectory::onFileWatchEvent);

	return true;
}

//-----------------------------------//

Stream* ArchiveDirectory::openFile(const Path& file, Allocator* alloc)
{
	Path fullPath = combinePath(file);

	if (!FileExists(fullPath))
		return nullptr;

	Stream* stream = Allocate(alloc, FileStream, fullPath, StreamOpenMode::Read);

	return stream;
}

//-----------------------------------//

static void DirArchiveEnumerate(Array<String>&, Path, Path, bool);

void  ArchiveDirectory::enumerateFiles(Array<Path>& paths)
{
	FileEnumerateFiles(path, paths);
}

//-----------------------------------//

void ArchiveDirectory::enumerateDirs(Array<Path>& paths)
{
	FileEnumerateDirectories(path, paths);
}

//-----------------------------------//

bool ArchiveDirectory::existsFile(const Path& path)
{
	Path normalized = PathNormalize(path);
	auto& fullPath = combinePath(normalized);
	return FileExists(fullPath);
}

//-----------------------------------//

bool ArchiveDirectory::existsDir(const Path& path)
{
	Array<Path> dirs;
	enumerateDirs(dirs);
	Path normalizedPath = PathNormalize(path);


	for(auto& i : dirs)
	{
		Path normalized = PathNormalize(i);
		Path dir = StringTrim(normalized, "/");
		if(dir == normalizedPath) return true;
	}

	return false;
}

//-----------------------------------//

void ArchiveDirectory::onFileWatchEvent(const FileWatchEvent& event)
{
	auto archive = (Archive*) event.userdata;
	if( archive != this )
		return;

	archive->onWatchEvent(this, event);
}

//-----------------------------------//

#endif

NAMESPACE_CORE_END