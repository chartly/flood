/************************************************************************
*
* Flood Project � (2008-201x)
* Licensed under the simplified BSD license. All rights reserved.
*
************************************************************************/

#include "Engine/API.h"
#include "Engine/Geometry/DebugGeometry.h"
#include "Core/Math/BoundingBox.h"
#include "Core/Math/Frustum.h"
#include "Core/Math/Color.h"
#include "Core/Memory.h"
#include "Graphics/GeometryBuffer.h"
#include "Graphics/RenderBatch.h"
#include "Engine/Scene/Geometry.h"
#include "Engine/Scene/Entity.h"
#include "Engine/Scene/Tags.h"

NAMESPACE_ENGINE_BEGIN

//-----------------------------------//

static void SetupDebugVertexFormat(GeometryBuffer* gb);

DebugDrawer::DebugDrawer()
	: lines(nullptr)
	, triangles(nullptr)
{
	Allocator* alloc = AllocatorGetThis();
	
	MaterialHandle debug = MaterialCreate(alloc, "Debug");
	
	Material* mat = debug.Resolve();
	mat->setBackfaceCulling(false);
	mat->setBlending(BlendSource::SourceAlpha, BlendDestination::InverseSourceAlpha);
	mat->setDepthCompare( DepthCompare::LessOrEqual );
	mat->setDepthOffset(Vector2(-1.0f, 1.0f));

	//mat->setDepthRange( Vector2(0.1f, 0.9f) );
	//mat->setDepthWrite(false);

	// Lines
	GeometryBuffer* linesVB = AllocateThis(GeometryBuffer);
	linesVB->setBufferAccess(BufferAccess::Write);
	linesVB->setBufferUsage(BufferUsage::Dynamic);
	SetupDebugVertexFormat(linesVB);

	lines = RenderBatchCreate(alloc);
	lines->setGeometryBuffer(linesVB);
	lines->setPrimitiveType(PrimitiveType::Lines);
	lines->setMaterial(debug);
	lines->setRenderLayer(RenderLayer::PostTransparency);
	renderables.pushBack(lines.get());

	// Triangles
	GeometryBufferPtr trianglesVB = AllocateThis(GeometryBuffer);
	trianglesVB->setBufferAccess(BufferAccess::Write);
	trianglesVB->setBufferUsage(BufferUsage::Dynamic);
	SetupDebugVertexFormat(trianglesVB.get());

	triangles = RenderBatchCreate(alloc);
	triangles->setGeometryBuffer(trianglesVB);
	triangles->setPrimitiveType(PrimitiveType::Triangles);
	triangles->setMaterial(debug);
	triangles->setRenderLayer(RenderLayer::PostTransparency);
	renderables.pushBack(triangles.get());

	// Quads
	GeometryBufferPtr quadsVB = AllocateThis(GeometryBuffer);
	quadsVB->setBufferAccess(BufferAccess::Write);
	quadsVB->setBufferUsage(BufferUsage::Dynamic);
	SetupDebugVertexFormat(quadsVB.get());

	quads = RenderBatchCreate(alloc);
	quads->setGeometryBuffer(quadsVB);
	quads->setPrimitiveType(PrimitiveType::Quads);
	quads->setMaterial(debug);
	quads->setRenderLayer(RenderLayer::PostTransparency);
	renderables.pushBack(quads.get());

	reset();
}

//-----------------------------------//

DebugDrawer::~DebugDrawer()
{

}

//-----------------------------------//

static void SetupDebugVertexFormat(GeometryBuffer* gb)
{
	// Setup the vertex format.
	gb->declarations.reset();

	VertexElementP elemPosition = { VertexAttribute::Position, VertexDataType::Float, 3 };
	gb->declarations.add(elemPosition);

	VertexElementP elemColor = { VertexAttribute::Color, VertexDataType::Float, 4 };
	gb->declarations.add(elemColor);

	gb->declarations.calculateStrides();
}

//-----------------------------------//

void DebugDrawer::reset()
{
	GeometryBuffer* linesGB = lines->getGeometryBuffer().get();
	linesGB->clear();

	GeometryBuffer* trianglesGB = triangles->getGeometryBuffer().get();
	trianglesGB->clear();

	GeometryBuffer* quadsGB = quads->getGeometryBuffer().get();
	quadsGB->clear();

	//renderables.clear();

	currentColor = Color::White;
	currentColor.a = 0.4f;
}

//-----------------------------------//

void DebugDrawer::setColor( const Color& newColor )
{
	currentColor = newColor;
}

//-----------------------------------//

void DebugDrawer::drawBox( const BoundingBox& box )
{
	if( box.min == Vector3::Zero && box.max == Vector3::Zero )
		return;

	if( box.isInfinite() )
		return;

	DebugUpdateBoudingBox(quads->getGeometryBuffer().get(), box, currentColor);
}

//-----------------------------------//

void DebugDrawer::drawRay( const Ray& ray, float length )
{
	RenderablePtr rend = DebugBuildRay(ray, length);
	//renderables.pushBack(rend);
}

//-----------------------------------//

void DebugDrawer::drawFrustum( const Frustum& frustum )
{
	RenderablePtr rend = DebugBuildFrustum(frustum);
	//renderables.pushBack(rend);
}

//-----------------------------------//

void DebugDrawer::drawIcon( const Vector3& pos )
{
}

//-----------------------------------//

RenderablePtr DebugBuildBoundingBox( const BoundingBox& box )
{
	GeometryBuffer* gb = AllocateHeap(GeometryBuffer);

	MaterialHandle materialHandle = MaterialCreate(AllocatorGetHeap(), "BoundingBoxDebug");
	
	Material* mat = materialHandle.Resolve();
	mat->setDepthCompare( DepthCompare::LessOrEqual );
	mat->setBackfaceCulling( false );
	//mat->setDepthRange( Vector2(0.1f, 0.9f) );

	Renderable* renderable = AllocateHeap(Renderable);
	renderable->setPrimitiveType(PrimitiveType::Quads);
	renderable->setGeometryBuffer(gb);
	renderable->setMaterial(materialHandle);
	renderable->setPrimitiveRasterMode( PrimitiveRasterMode::Wireframe );

	DebugUpdateBoudingBox(gb, box, Color::White);

	return renderable;
}

#define ADD_BOX_FACE( a, b, c, d ) \
	vs[i++].pos = ( box.getCorner(a) ); \
	vs[i++].pos = ( box.getCorner(b) ); \
	vs[i++].pos = ( box.getCorner(c) ); \
	vs[i++].pos = ( box.getCorner(d) );

void DebugUpdateBoudingBox( GeometryBuffer* gb, const BoundingBox& box, Color color )
{
	struct Vertex
	{
		Vector3 pos;
		Color color;
	};

	Array<Vertex> vs;
	vs.resize(24);

	size_t i = 0;
	ADD_BOX_FACE( 0, 2, 3, 1 ) // Front
	ADD_BOX_FACE( 0, 1, 5, 4 ) // Bottom
	ADD_BOX_FACE( 4, 5, 7, 6 ) // Back
	ADD_BOX_FACE( 2, 6, 7, 3 ) // Top
	ADD_BOX_FACE( 0, 4, 6, 2 ) // Left
	ADD_BOX_FACE( 1, 3, 7, 5 ) // Right

	for(i = 0; i < vs.size(); i++)
	{
		vs[i].color = color;
	}

	gb->add((uint8*)vs.data(), vs.size()*sizeof(Vertex));

	gb->forceRebuild();
}

//-----------------------------------//

RenderablePtr DebugBuildRay( const Ray& pickRay, float length )
{
	Array<Vector3> vertex;
	vertex.pushBack( pickRay.origin );
	vertex.pushBack( pickRay.getPoint(length) );

	Array<Vector3> colors( 2, Color::Red );

	GeometryBuffer* gb = AllocateHeap(GeometryBuffer);
	gb->set( VertexAttribute::Position, vertex );
	gb->set( VertexAttribute::Color, colors );

	MaterialHandle material = MaterialCreate(AllocatorGetHeap(), "RayDebug");

	Renderable* renderable = AllocateHeap(Renderable);
	renderable->setPrimitiveType(PrimitiveType::Lines);
	renderable->setGeometryBuffer(gb);
	renderable->setMaterial(material);
	
	return renderable;
}

//-----------------------------------//

RenderablePtr DebugBuildFrustum( const Frustum& box )
{
	GeometryBuffer* gb = AllocateHeap(GeometryBuffer);

	MaterialHandle materialHandle = MaterialCreate(AllocatorGetHeap(), "FrustumDebug");
	Material* material = materialHandle.Resolve();
	material->setBackfaceCulling( false );

	Renderable* renderable = AllocateHeap(Renderable);
	renderable->setPrimitiveType(PrimitiveType::Quads);
	renderable->setGeometryBuffer(gb);
	renderable->setMaterial(materialHandle);
	renderable->setPrimitiveRasterMode( PrimitiveRasterMode::Wireframe );

	DebugUpdateFrustum(renderable, box);
	return renderable;
}

//-----------------------------------//

#define ADD_BOX_FRUSTUM( a, b, c, d ) \
	pos.pushBack( box.corners[a] ); \
	pos.pushBack( box.corners[b] ); \
	pos.pushBack( box.corners[c] ); \
	pos.pushBack( box.corners[d] );

void DebugUpdateFrustum( const RenderablePtr& rend, const Frustum& box )
{
	Array<Vector3> pos;
	ADD_BOX_FRUSTUM( 0, 1, 3, 2 ) // Front
	ADD_BOX_FRUSTUM( 0, 1, 5, 4 ) // Top
	ADD_BOX_FRUSTUM( 4, 5, 7, 6 ) // Back
	ADD_BOX_FRUSTUM( 2, 3, 7, 6 ) // Bottom
	ADD_BOX_FRUSTUM( 0, 2, 6, 4 ) // Left
	ADD_BOX_FRUSTUM( 5, 1, 3, 7 ) // Right

	GeometryBuffer* gb = rend->getGeometryBuffer().get();
	gb->set( VertexAttribute::Position, pos );

	Array<Vector3> colors( pos.size(), Color::White );
	gb->set( VertexAttribute::Color, colors );

	gb->forceRebuild();
}

//-----------------------------------//

NAMESPACE_ENGINE_END