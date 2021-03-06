/************************************************************************
*
* Flood Project � (2008-201x)
* Licensed under the simplified BSD license. All rights reserved.
*
************************************************************************/

#include "Engine/API.h"
#include "Engine/Geometry/Quad.h"

NAMESPACE_ENGINE_BEGIN

//-----------------------------------//

REFLECT_CHILD_CLASS(Quad, Geometry)
REFLECT_CLASS_END()

//-----------------------------------//

Quad::Quad( float width, float height )
{
	const GeometryBufferPtr& gb = createQuad(width, height);

	MaterialHandle mat = MaterialCreate(AllocatorGetThis(), "Quad");
	mat.Resolve()->setBackfaceCulling(false);

	RenderablePtr rend = RenderBatchCreate( AllocatorGetThis() );
	rend->setPrimitiveType(PrimitiveType::TriangleStrip);
	rend->setGeometryBuffer(gb);
	rend->setMaterial(mat);

	addRenderable(rend);
}

//-----------------------------------//

GeometryBufferPtr Quad::createQuad( float width, float height )
{
	// Create a new buffer and store triangle data.
	GeometryBufferPtr gb = AllocateThis(GeometryBuffer);

	// Vertex position data
	Array< Vector3 > vertex;
	vertex.pushBack( Vector2(0.0f, 0.0f) );
	vertex.pushBack( Vector2(width, 0.0f) );
	vertex.pushBack( Vector2(0.0f, height) );
	vertex.pushBack( Vector2(width, height) );

	// Vertex color data
	Array< Vector3 > colors;
	colors.pushBack( Color::White );
	colors.pushBack( Color::White );
	colors.pushBack( Color::White );
	colors.pushBack( Color::White );

	// Vertex tex coords data
	Array< Vector3 > coords;
	coords.pushBack( Vector2(0.0f, 1.0f) );
	coords.pushBack( Vector2(1.0f, 0.0f) );
	coords.pushBack( Vector2(1.0f, 1.0f) );
	coords.pushBack( Vector2(0.0f, 1.0f) );

	// Vertex buffer setup
	gb->set( VertexAttribute::Position, vertex );
	gb->set( VertexAttribute::Color, colors );
	gb->set( VertexAttribute::TexCoord0, coords );

	return gb;
}

//-----------------------------------//

NAMESPACE_ENGINE_END