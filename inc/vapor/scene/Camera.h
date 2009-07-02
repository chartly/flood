/************************************************************************
*
* vaporEngine (2008-2009)
*
*	<http://www.portugal-a-programar.org>
*
************************************************************************/

#pragma once

#include "vapor/math/Vector3.h"

namespace vapor {
	namespace render {

/**
 * Represents a view from a specific point in the world. Has an associated 
 * projection type, like ortographic or perspective and also holds a frustum 
 * that will be used for performing viewing frustum culling. Culling helps
 * speed up the rendering by cutting nodes that are outside of the view range.
 */

class Camera
{
public:

	Camera();
	virtual ~Camera();

	void lookAt(math::Vector3 pos);

private:

	// Frustum frustum;
	int zoomX, zoomY;
};

} } // end namespaces