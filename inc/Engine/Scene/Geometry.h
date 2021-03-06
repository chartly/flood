/************************************************************************
*
* Flood Project � (2008-201x)
* Licensed under the simplified BSD license. All rights reserved.
*
************************************************************************/

#pragma once

#include "Engine/Scene/Transform.h"
#include "Core/Math/BoundingBox.h"
#include "Graphics/RenderBatch.h"
#include "Graphics/RenderQueue.h"

NAMESPACE_ENGINE_BEGIN

//-----------------------------------//

/**
 * This component holds geometry data for rendering. It can hold multiple
 * renderable objects, each with different materials. When the camera node
 * traverses the scene graph, Geometry nodes are picked and passed to the
 * rendering device for rendering.
 */

API_SCENE REFLECT_DECLARE_CLASS(Geometry)

class API_SCENE Geometry : public Component
{
	REFLECT_DECLARE_OBJECT(Geometry)

public:

	Geometry();

	// Adds a new.renderable to this geometry.
	void addRenderable( const RenderBatchPtr& renderable );

	// Gets all the renderables in this geometry.
	RenderablesVector getRenderables() const;

	// Appends all the renderables of this geometry to the queue.
	virtual void appendRenderables(
		RenderQueue& queue, const Transform* transform);

	// Updates the geometry bounds if needed.
	virtual void updateBounds();

	// Updates some geometry information.
	virtual void update( float delta ) OVERRIDE;

	// Gets the bounding volume of this geometry.
	GETTER(BoundingVolume, const BoundingBox&, bounds)

	// Gets the world bounding volume of the transform.
	BoundingBox getWorldBoundingVolume() const;

	// Marks the geometry as dirty (forces bounding volume update).
	void rebuildBoundingBox();

protected:

	// Notifies the transform of rebuilt bounding volumes.
	void notifiesTransform();

	// Renderables of the geometry.
	RenderablesVector renderables;

	// Bounding volume of the geometry.
	BoundingBox bounds;

	// Keeps track if bounds needs to be rebuilt.
	bool needsBoundsRebuild;
};

TYPEDEF_INTRUSIVE_POINTER_FROM_TYPE( Geometry );

//-----------------------------------//

NAMESPACE_ENGINE_END