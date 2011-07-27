/************************************************************************
*
* vapor3D Engine � (2008-2010)
*
*	<http://www.vapor3d.org>
*
************************************************************************/

%module vapor

%begin %{
#include "Engine/API.h"
#include "Headers.h"
using namespace vapor;
%}

%import "base.i"

namespace vapor
{
	VAPOR_API void LogDebug(const char* msg, ...);
	VAPOR_API void LogInfo(const char* msg, ...);
	VAPOR_API void LogWarn(const char* msg, ...);
	VAPOR_API void LogError(const char* msg, ...);
}

%include "Math.i"
%include "Resources.i"
%include "Input.i"
%include "Render.i"
%include "Scene.i"

//%include "Engine.h"
