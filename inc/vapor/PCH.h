/************************************************************************
*
* vapor3D Engine � (2008-2010)
*
*	<http://www.vapor3d.org>
*
************************************************************************/

#pragma once

//---------------------------------------------------------------------//
// Compile-time options
//---------------------------------------------------------------------//

#include "vapor/CompileOptions.h"

//---------------------------------------------------------------------//
// Platform headers
//---------------------------------------------------------------------//

#include "vapor/Platform.h"

//---------------------------------------------------------------------//
// Language features (C++)
//---------------------------------------------------------------------//

#include "vapor/Language.h"

//---------------------------------------------------------------------//
// Utilities
//---------------------------------------------------------------------//

#include "vapor/Utilities.h"

//---------------------------------------------------------------------//
// Logging. Use and abuse it.
//---------------------------------------------------------------------//

#include "vapor/Log.h"

using vapor::log::info;
using vapor::log::warn;
using vapor::log::error;
using vapor::log::debug;

//---------------------------------------------------------------------//
// Profiling
//---------------------------------------------------------------------//

#include "vapor/Profiler.h"

//---------------------------------------------------------------------//
// Reference Counting
//---------------------------------------------------------------------//

#include "vapor/ReferenceCounted.h"