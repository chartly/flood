/************************************************************************
*
* Flood Project � (2008-201x)
* Licensed under the simplified BSD license. All rights reserved.
*
************************************************************************/

#include "Graphics/API.h"

#ifdef ENABLE_RENDERER_OPENGL

#include "Graphics/RenderDevice.h"
#include "Graphics/RenderContext.h"
#include "Graphics/RenderBackend.h"
#include "Graphics/RenderBatch.h"
#include "Graphics/RenderView.h"
#include "Graphics/ShaderProgram.h"
#include "Graphics/GeometryBuffer.h"

#include "Graphics/BufferManager.h"
#include "Graphics/ShaderProgramManager.h"
#include "Graphics/TextureManager.h"

#include "Core/Utilities.h"
#include <algorithm>

NAMESPACE_GRAPHICS_BEGIN

//-----------------------------------//

REFLECT_ENUM(RenderPipeline)
	ENUM(Fixed)
	ENUM(ShaderForward)
	ENUM(ShaderDeferred)
REFLECT_ENUM_END()

//-----------------------------------//

static RenderDevice* gs_RenderDevice = nullptr;
RenderDevice* GetRenderDevice() { return gs_RenderDevice; }

RenderDevice::RenderDevice()
	: activeContext(nullptr)
	, activeTarget(nullptr)
	, activeView(nullptr)
	, renderBackend(nullptr)
	//, shadowDepthBuffer(nullptr)
	, pipeline(RenderPipeline::ShaderForward)
{
	gs_RenderDevice = this;
}

//-----------------------------------//

RenderDevice::~RenderDevice()
{
}

//-----------------------------------//

void RenderDevice::updateRenderTargets()
{
}

//-----------------------------------//

static bool RenderStateSorter(const RenderState& lhs, const RenderState& rhs)
{
	int rA = (int) lhs.renderable->getRenderLayer();
	int rB = (int) rhs.renderable->getRenderLayer();
	int pA = lhs.priority;
	int pB = rhs.priority;

	return (rA == rB) ? (pA < pB) : (rA < rB);
}

//-----------------------------------//

void RenderDevice::render( RenderBlock& queue ) 
{
	#pragma TODO("Sort the render group by depth distance")

	// Sort the renderables by render group.
	std::sort( queue.renderables.begin(), queue.renderables.end(), &RenderStateSorter );

	// Render all the renderables in the queue.
	for( size_t i = 0; i < queue.renderables.size(); i++ )
	{
		const RenderState& state = queue.renderables[i];
		render(state, queue.lights);
	}
}

//-----------------------------------//

void RenderDevice::render( const RenderState& state, const LightQueue& lights )
{
	BufferManager* buffers = activeContext->bufferManager;
	ProgramManager* programs = activeContext->programManager;
	
	RenderBatch* renderable = state.renderable;
	bindBuffers(renderable);

	const GeometryBuffer* gb = renderable->getGeometryBuffer().get();
	if( gb->data.empty() ) return;

	BufferEntry* bufs = buffers->getBuffer(gb);

	// Setup the vertex buffer format.
	VertexBuffer* vb = bufs->vb.get();
	renderBackend->setupVertexBuffer(vb);
	
	Material* material = state.material;
	ShaderMaterial* shader = material->getShader().Resolve();

	ShaderProgram* shaderProgram = programs->getProgram(shader);
	if( !shaderProgram ) return;

	if( !shaderProgram->isLinked() && !shaderProgram->link() )
		return;

	shaderProgram->bind();

	renderBackend->setupRenderState(state, true);
	bindTextureUnits(state, true);

	if( !renderable->onPreRender.empty() )
	{
		// Call the user pre render hook.
		renderable->onPreRender(activeView, state);
	}

	RenderLayer stage = renderable->getRenderLayer();

	if( stage != RenderLayer::Overlays )
	{
		if( !setupRenderStateMatrix(state) )
			return;

		//if( !setupRenderStateLight(state, lights) )
		//	return;
	}
	else if( stage == RenderLayer::Overlays )
	{
		if( !setupRenderStateOverlay(state) )
			return;
	}

	UniformBuffer* ub = renderable->getUniformBuffer().get();
	shaderProgram->setUniforms(ub);

	renderBackend->renderBatch(renderable);
	
	if( !renderable->onPostRender.empty() )
	{
		// Call the user post render hook.
		renderable->onPostRender(activeView, state);
	}
	
	renderBackend->unsetupRenderState(state);
	unbindTextureUnits(state.material);
	
	shaderProgram->unbind();

	renderBackend->unbindVertexBuffer(vb);
	unbindBuffers(renderable);
}

//-----------------------------------//

void RenderDevice::bindTextureUnits(const RenderState& state, bool bindUniforms)
{
	TextureUnitMap& units = state.material->textureUnits;
	UniformBuffer* ub = state.renderable->getUniformBuffer().get();

	for( auto it = units.begin(); it != units.end(); it++ )
	{
		auto& unit = it->value;
		auto& handle = unit.image;

		Texture* texture = activeContext->textureManager->getTexture(handle).get();
		if( !texture ) continue;

		if( !texture->isUploaded() )
		{
			renderBackend->uploadTexture(texture);
			renderBackend->configureTexture(texture);
		}

		renderBackend->bindTexture(texture);
		renderBackend->setupTextureUnit(texture, unit);
		
		if( !bindUniforms ) continue;

		char s_TextureUniform[] = "vp_Texture0";
		size_t s_TextureUniformSize = FLD_ARRAY_SIZE(s_TextureUniform) - 2;

		// Build the uniform string without allocating memory.
		uint8 index = unit.unit;
		char indexChar = (index + '0');
		
		s_TextureUniform[s_TextureUniformSize] = indexChar;
		ub->setUniform( s_TextureUniform, (int32) index );
	}
}

//-----------------------------------//

void RenderDevice::unbindTextureUnits(Material* material)
{
	TextureUnitMap& units = material->textureUnits;
	TextureManager* textureManager = activeContext->textureManager;

	for( auto it = units.begin(); it != units.end(); it++ )
	{
		auto& unit = it->value;
		auto& handle = unit.image;

		Texture* texture = textureManager->getTexture(handle).get();
		if( !texture ) continue;

		renderBackend->setupTextureUnit(texture, unit);
		renderBackend->unbindTexture(texture);
	}
}

//-----------------------------------//

bool RenderDevice::setupRenderStateMatrix( const RenderState& state )
{
	const Matrix4x3& matModel = state.modelMatrix;
	const Matrix4x3& matView = activeView->viewMatrix;
	const Matrix4x4& matProjection = activeView->projectionMatrix;

	UniformBuffer* ub = state.renderable->getUniformBuffer().get();
	ub->setUniform( "vp_ModelMatrix", matModel );
	ub->setUniform( "vp_ViewMatrix", matView );
	ub->setUniform( "vp_ProjectionMatrix", matProjection );

	return true;
}

//-----------------------------------//

bool RenderDevice::bindBuffers(RenderBatch* renderable)
{
	BufferManager* buffers = activeContext->bufferManager;
	GeometryBuffer* gb = renderable->getGeometryBuffer().get();

	VertexBuffer* vb = buffers->getVertexBuffer(gb).get();
	IndexBuffer* ib = buffers->getIndexBuffer(gb).get();
	
	if( !vb ) return false;

	if( !vb->isBuilt() || gb->needsRebuild )
	{
		// If the vertex buffer is not built yet, then we build it.
		renderBackend->buildVertexBuffer(vb);
	}

	renderBackend->bindVertexBuffer(vb);

	// If there is no index buffer associated with the geometry, we are done.
	if( !ib ) goto done;

	renderBackend->bindIndexBuffer(ib);
	
	if( !ib->isBuilt || gb->needsRebuild )
	{
		// If the index buffer is not built, we also need to build it.
		renderBackend->buildIndexBuffer(ib);
	}

done:

	gb->needsRebuild = false;
	return true;
}

//-----------------------------------//

bool RenderDevice::unbindBuffers(RenderBatch* renderable)
{
	BufferManager* buffers = activeContext->bufferManager;
	const GeometryBuffer* gb = renderable->getGeometryBuffer().get();
	
	VertexBuffer* vb = buffers->getVertexBuffer(gb).get();
	if( !vb ) return false;
	
	renderBackend->unbindVertexBuffer(vb);

	IndexBuffer* ib = buffers->getIndexBuffer(gb).get();
	if( ib ) renderBackend->unbindIndexBuffer(ib);

	return true;
}

//-----------------------------------//
#if 0
void RenderDevice::updateLightDepth( LightState& state )
{
#if 0
	const LightPtr& light = state.light;
	assert( light->getLightType() == LightType::Directional );

	Texture* shadowDepthTexture;
	
	if( !shadowDepthBuffer )
	{
		shadowDepthBuffer = createRenderBuffer( Settings(512, 512) );
	}

	if( shadowTextures.find(light) == shadowTextures.end() )
	{
		shadowDepthTexture = shadowDepthBuffer->createRenderTexture();
		shadowTextures[light] = shadowDepthTexture;
	}
	else
	{
		shadowDepthTexture = shadowTextures[light];
	}

	CameraPtr lightCamera( new Camera(*camera) );
	TransformPtr lightTransform( new Transform(*state.transform.get()) );
	
	EntityPtr lightCameraEntity( new Entity("ShadowCamera") );
	lightCameraEntity->addTransform(); /*Component( lightTransform );*/
	lightCameraEntity->addComponent( lightCamera );

	RenderView* lightView = new View(lightCamera, shadowDepthBuffer);

	if( !shadowDepthBuffer->check() )
		return;

	// TODO: turn off color writes (glColorMask?)
	lightView->update();
	shadowDepthBuffer->unbind();

	Matrix4x4 bias;
	bias.identity();
	bias.m11 = 0.5f;
	bias.m22 = 0.5f;
	bias.m33 = 0.5f;
	bias.tx  = 0.5f;
	bias.ty  = 0.5f;
	bias.tz  = 0.5f;

	const Frustum& frustum = lightCamera->getFrustum();
	const Matrix4x4& matProjection = frustum.projectionMatrix;

	state.projection = lightCamera->getViewMatrix()
		* matProjection * bias;
#endif
}

//-----------------------------------//

bool RenderDevice::setupRenderStateShadow( LightQueue& lights )
{
#if 0
	if( lights.empty() ) return true;

	for( size_t i = 0; i < lights.size(); i++ )
	{
		LightState& state = lights[i];
		const Light* light = state.light;

		if( !light->getCastsShadows() )
			continue;

		updateLightDepth( state );
	}
#endif

	return true;
}

//-----------------------------------//

bool RenderDevice::setupRenderStateLight( const RenderState& state, const LightQueue& lights )
{
#if 0
	const UniformBufferPtr& ub = state.renderable->getUniformBuffer();

	for( size_t i = 0; i < lights.size(); i++ )
	{
		const LightState& lightState = lights[i];
		const Light* light = lightState.light;

		//TexturePtr shadowDepthTexture;
		//shadowDepthTexture = shadowTextures[light];
		//assert( shadowDepthTexture != nullptr );

		Array< Color > colors;
		colors.pushBack( light->getDiffuseColor() );
		colors.pushBack( light->getSpecularColor() );
		colors.pushBack( light->getEmissiveColor() );
		colors.pushBack( light->getAmbientColor() );

		const Transform* transform = lightState.transform;

		// TODO: fix the lighting stuff
		ub->setUniform( "vp_LightColors", colors );
		ub->setUniform( "vp_LightDirection", transform->getRotationMatrix() );
		//ub->setUniform( "vp_ShadowMap", shadowDepthTexture->id() );
		//ub->setUniform( "vp_CameraProj", state.modelMatrix * lightState.projection );
	}
#endif

	return true;
}
#endif
//-----------------------------------//

bool RenderDevice::setupRenderStateOverlay( const RenderState& state )
{
	Vector2i size = activeTarget->getSettings().getSize();
	Matrix4x4 projection = Matrix4x4::createOrthographic(0, size.x, size.y, 0, -100, 100);

	const UniformBufferPtr& ub = state.renderable->getUniformBuffer();
	ub->setUniform( "vp_ProjectionMatrix", projection );
	ub->setUniform( "vp_ModelMatrix", state.modelMatrix );
	ub->setUniform( "vp_ViewMatrix", Matrix4x4::Identity );
	ub->setUniform( "vp_ModelViewMatrix", state.modelMatrix );

	return true;
}

//-----------------------------------//

void RenderDevice::setActiveView( RenderView* view )
{
	activeView = view;
	if( !activeView ) return;

	setRenderTarget(view->getRenderTarget());
	renderBackend->setupRenderView(view);
}

//-----------------------------------//

void RenderDevice::clearView()
{
	renderBackend->clearRenderView(activeView);
}

//-----------------------------------//

void RenderDevice::setRenderTarget(RenderTarget* target)
{
	activeTarget = target;
	if( !activeTarget ) return;

	//activeTarget->makeCurrent();
	activeContext = activeTarget->getContext();

	renderBackend = activeContext->backend;
}

//-----------------------------------//

bool RenderDevice::isFixedPipeline() const
{
	return pipeline == RenderPipeline::Fixed;
}

//-----------------------------------//

NAMESPACE_GRAPHICS_END

#endif