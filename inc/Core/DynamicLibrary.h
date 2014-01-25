/************************************************************************
*
* Flood Project � (2008-201x)
* Licensed under the simplified BSD license. All rights reserved.
*
************************************************************************/

#pragma once

#ifdef ENABLE_DYNAMIC_LIBRARY

NAMESPACE_CORE_BEGIN

//-----------------------------------//

#ifdef PLATFORM_WINDOWS
    typedef void* DynLibHandle;
#else
    #error "Support for dynamic libraries not found"
#endif

//-----------------------------------//

/**
 * Dynamic library that can be loaded at runtime (DLLs on Windows,
 * SO on *nix, DyLib on MacOS X). After the library is loaded, it is 
 * possible to retrieve symbols using this class.
 */

class CORE_API DynamicLibrary
{
public:
    DynamicLibrary( const String& name );
    DynamicLibrary(const DynamicLibrary&) = delete;
    ~DynamicLibrary();
    DynamicLibrary& operator=(const DynamicLibrary&) = delete;

    // Gets a pointer to the given symbol.
    void* getSymbol( const String& symbol );

private:

    // Loads the dynamic library.
    bool load();
    
    String _name;
    DynLibHandle _handle;
};

//-----------------------------------//

NAMESPACE_CORE_END

#endif