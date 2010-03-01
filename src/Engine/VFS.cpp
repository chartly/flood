/************************************************************************
*
* vapor3D Engine � (2008-2010)
*
*	<http://www.vapor3d.org>
*
************************************************************************/

#include "vapor/PCH.h"
#include "vapor/vfs/WatcherWin32.h"

#ifdef VAPOR_VFS_PHYSFS

#include "vapor/vfs/VFS.h"
#include <physfs.h>

namespace vapor { namespace vfs {

//-----------------------------------//

VFS::VFS(const std::string& app, const char* argv0 )
{
	int err = PHYSFS_init( argv0 );

	if( err == 0 ) 
	{
		error( "vfs",
			"Could not initialize PhysFS: %s", PHYSFS_getLastError() );
		
		return;
	}

	setDefaultConfig(app);
	log();

#ifdef VAPOR_PLATFORM_WINDOWS
	watcher = new WatcherWin32();
#endif

	mountDefaultLocations();
}

//-----------------------------------//

VFS::~VFS()
{
	foreach( const std::string& point, mountPoints )
	{
		PHYSFS_removeFromSearchPath( point.c_str() );
	}
	
	mountPoints.clear();

	int err = PHYSFS_deinit();

	if( err == 0 )
	{
		error( "vfs", "Could not clean up PhysFS: %s", PHYSFS_getLastError() );
	}

	delete watcher;
}

//-----------------------------------//

void VFS::setDefaultConfig(const std::string& app)
{
	int err = PHYSFS_setSaneConfig("vapor", app.c_str(), nullptr, 0, 0);

	if( err == 0 ) 
	{
		error( "vfs",
			"Could not set a sane config: %s", PHYSFS_getLastError() );
	}

	info( "vfs", "Mounted '%s' in mount point '/'", PHYSFS_getBaseDir() );
}

//-----------------------------------//

void VFS::log()
{
	std::stringstream ss;

	PHYSFS_Version version;
	PHYSFS_getLinkedVersion(&version);

	info("vfs", "Initialized PhysFS version %d.%d.%d", 
		version.major, version.minor, version.patch);

	const PHYSFS_ArchiveInfo **i;

	ss << "Supported archives: ";

	for (i = PHYSFS_supportedArchiveTypes(); *i != nullptr; i++)
	{
		ss << "'" << (*i)->extension << "', ";
			// << "' (" << (*i)->description << "), ";
	}

	info( "vfs", "User write folder: %s", PHYSFS_getWriteDir() );
	
	std::string s = ss.str();
	s = s.substr( 0, s.find_last_of( ',' ) );
	info( "vfs", "%s", s.c_str() );
}

//-----------------------------------//

bool VFS::mount(const std::string& path, const std::string& mount, bool append )
{
	int err = PHYSFS_mount( path.c_str(), mount.c_str(), append ? 1 : 0 );

	if( err == 0 ) 
	{
		error( "vfs", "Could not mount '%s': %s", 
			path.c_str(), PHYSFS_getLastError() );

		return false;
	}

	mountPoints.push_back( path );

	info( "vfs", "Mounted '%s' in mount point '%s'",
		path.c_str(), mount.empty() ? "/" : mount.c_str() );

	watcher->addWatch( path );

	return true;
}

//-----------------------------------//

void VFS::mountDefaultLocations()
{
	// Default VFS mount points
	const std::string& media = "media/";

	std::string dirs[] = 
	{
		"fonts",
		"images",
		"materials",
		"meshes",
		"scripts",
		"sounds"
	};

	if ( !mount( media ) )
		return;

	foreach( const std::string& dir, dirs )
		mount(media+dir);
}

//-----------------------------------//

const std::vector< std::string >& VFS::getMountPoints() const
{
	return mountPoints;
}

//-----------------------------------//

Watcher* VFS::getWatcher() const
{
	return watcher;
}

//-----------------------------------//

} } // end namespaces

#endif