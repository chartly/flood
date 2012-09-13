/************************************************************************
*
* vapor3D Engine � (2008-2010)
*
*	<http://www.vapor3d.org>
*
************************************************************************/

#pragma once

#include "Engine/Scene/Geometry.h"
#include "Engine/Geometry/Quad.h"

NAMESPACE_ENGINE_BEGIN

//-----------------------------------//

/**
 * Water plane.
 */

API_SCENE REFLECT_DECLARE_CLASS(Water)

class API_SCENE Water : public Geometry
{
public:

	Water();
	Water( MaterialHandle material );

protected:

	// Water plane.
	QuadPtr quad;
};

TYPEDEF_INTRUSIVE_POINTER_FROM_TYPE( Water );

//-----------------------------------//

NAMESPACE_ENGINE_END