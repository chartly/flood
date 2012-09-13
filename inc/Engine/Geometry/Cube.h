/************************************************************************
*
* vapor3D Engine � (2008-2010)
*
*	<http://www.vapor3d.org>
*
************************************************************************/

#pragma once

#include "Engine/Scene/Geometry.h"

NAMESPACE_ENGINE_BEGIN

//-----------------------------------//

/**
 * Generates a procedural cube mesh.
 */

API_SCENE REFLECT_DECLARE_CLASS(Cube)

class API_SCENE Cube : public Geometry
{
	REFLECT_DECLARE_OBJECT(Cube)

public:

	Cube();
	Cube( float width, float height );

	// Creates the cube.
	void create();

private:

	float width;
	float height;
};

API_SCENE void BuildCube( GeometryBuffer* vb, float width, float height );

TYPEDEF_INTRUSIVE_POINTER_FROM_TYPE( Cube );

//-----------------------------------//

NAMESPACE_ENGINE_END
