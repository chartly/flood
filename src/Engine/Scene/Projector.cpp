/************************************************************************
*
* Flood Project � (2008-201x)
* Licensed under the simplified BSD license. All rights reserved.
*
************************************************************************/

#include "Engine/API.h"
#include "Engine/Scene/Projector.h"
#include "Engine/Scene/Transform.h"
#include "Engine/Scene/Entity.h"
#include "Engine/Geometry/DebugGeometry.h"
#include "Graphics/RenderDevice.h"
#include "Graphics/ShaderProgram.h"
#include "Graphics/ShaderProgramManager.h"

NAMESPACE_ENGINE_BEGIN

//-----------------------------------//

REFLECT_CHILD_CLASS(Projector, Geometry)
	FIELD_CLASS(4, Frustum, frustum)
	FIELD_CLASS_PTR(5, Material, MaterialHandle, material, Handle)
REFLECT_CLASS_END()

//-----------------------------------//

Projector::Projector()
{

}

//-----------------------------------//

Projector::~Projector()
{
	if( !geometry ) return;

	const RenderablesVector& renderables = geometry->getRenderables();
	
	for( size_t i = 0; i < renderables.size(); i++ )
	{
		RenderBatch* renderable = renderables[i].get();
		renderable->onPreRender.clear();

		const UniformBufferPtr& ub = renderable->getUniformBuffer();
		ub->removeUniform("vp_TextureProjection");
		ub->removeUniform("vp_TextureView");
	}
}

//-----------------------------------//

void Projector::update(float)
{
	const Transform* transform = getEntity()->getTransform().get();

	frustum.updateProjection();
	frustum.updateCorners(transform->getAbsoluteTransform());

	updateDebugRenderable();
}

//-----------------------------------//
 
void Projector::appendRenderables( RenderQueue& queue, const Transform* transform )
{
	if( !geometry ) return;

	const Transform* geotransform = geometry->getEntity()->getTransform().get();
	const Matrix4x3& absoluteTransform = geotransform->getAbsoluteTransform();
	
	const RenderablesVector& renderables = geometry->getRenderables();
	
	for( size_t i = 0; i < renderables.size(); i++ )
	{
		RenderBatch* renderable = renderables[i].get();
		if( !renderable ) continue;

		renderable->onPreRender.Bind(this, &Projector::onPreRender);

		RenderState state;
		state.renderable = renderable;
		state.material = material.Resolve();
		state.modelMatrix = absoluteTransform;
		state.priority = renderable->getRenderPriority() + 1;

		queue.pushBack(state);
	}
}

//-----------------------------------//

void Projector::onPreRender( RenderView*, const RenderState& state )
{
	const Transform* transform = getEntity()->getTransform().get();
	const Matrix4x4& absoluteTransform = transform->getAbsoluteTransform();

	const UniformBufferPtr& ub = state.renderable->getUniformBuffer();
	ub->setUniform("vp_TextureProjection", frustum.matProjection);
	ub->setUniform("vp_TextureView", absoluteTransform.inverse());
}

//-----------------------------------//

void Projector::updateDebugRenderable() const
{
	if( !debugRenderable ) return;
	DebugUpdateFrustum( debugRenderable, frustum );
}

//-----------------------------------//

RenderBatchPtr Projector::createDebuRenderable() const
{
	assert( !debugRenderable );

	debugInheritsTransform = true;
	return DebugBuildFrustum( frustum );
}

//-----------------------------------//

NAMESPACE_ENGINE_END