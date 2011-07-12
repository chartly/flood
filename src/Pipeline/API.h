/************************************************************************
*
* vapor3D Engine � (2008-2010)
*
*	<http://www.vapor3d.org>
*
************************************************************************/

#pragma once

//---------------------------------------------------------------------//
// Platform headers
//---------------------------------------------------------------------//

#include "Core/API.h"
#include "Resources/API.h"

//---------------------------------------------------------------------//
// API Exports
//---------------------------------------------------------------------//

#if defined(API_PIPELINE_DLL) && defined(API_PIPELINE_DLL_EXPORT)
	#define API_PIPELINE API_EXPORT
#elif defined(API_PIPELINE_DLL)
	#define API_PIPELINE API_IMPORT
#else
	#define API_PIPELINE
#endif

#define PIPELINE_API API_PIPELINE