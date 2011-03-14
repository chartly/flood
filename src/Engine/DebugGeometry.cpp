/************************************************************************
*
* vapor3D Engine � (2008-2010)
*
*	<http://www.vapor3d.org>
*
************************************************************************/

#include "Engine/API.h"
#include "Render/DebugGeometry.h"
#include "Render/Renderable.h"
#include "Scene/Geometry.h"
#include "Scene/Entity.h"
#include "Scene/Tags.h"

namespace vapor {

//-----------------------------------//

#define ADD_BOX_FACE( a, b, c, d )		\
	pos.push_back( box.getCorner(a) );	\
	pos.push_back( box.getCorner(b) );	\
	pos.push_back( box.getCorner(c) );	\
	pos.push_back( box.getCorner(d) );

RenderablePtr buildBoundingRenderable( const BoundingBox& box )
{
	std::vector<Vector3> pos;
	ADD_BOX_FACE( 0, 2, 3, 1 ) // Front
	ADD_BOX_FACE( 0, 1, 5, 4 ) // Bottom
	ADD_BOX_FACE( 4, 5, 7, 6 ) // Back
	ADD_BOX_FACE( 2, 6, 7, 3 ) // Top
	ADD_BOX_FACE( 0, 4, 6, 2 ) // Left
	ADD_BOX_FACE( 1, 3, 7, 5 ) // Right

	const int numColors = 6*4; // Faces*Vertices
	std::vector<Vector3> colors( numColors, Color::White );

	VertexBufferPtr vb( new VertexBuffer() );
	vb->set( VertexAttribute::Position, pos );
	vb->set( VertexAttribute::Color, colors );

	MaterialPtr mat( new Material("BoundingBoxDebug") );
	mat->setDepthCompare( DepthCompare::LessOrEqual );
	mat->setBackfaceCulling( false );

	RenderablePtr renderable = new Renderable(PolygonType::Quads);
	renderable->setVertexBuffer(vb);
	renderable->setMaterial(mat);
	renderable->setPolygonMode( PolygonMode::Wireframe );

	return renderable;
}

//-----------------------------------//

EntityPtr buildRay( const Ray& pickRay, const Vector3& outFar )
{
	std::vector<Vector3> vertex;
	vertex.push_back( pickRay.origin );
	vertex.push_back( outFar );

	std::vector<Vector3> colors( 2, Color::Red );

	VertexBufferPtr vb( new VertexBuffer() );
	vb->set( VertexAttribute::Position, vertex );
	vb->set( VertexAttribute::Color, colors );

	MaterialPtr mat = new Material("RayDebug");

	RenderablePtr renderable = new Renderable(PolygonType::Lines);
	renderable->setVertexBuffer(vb);
	renderable->setMaterial(mat);
	
	GeometryPtr geometry( new Geometry(renderable) );
	
	EntityPtr line( new Entity("Line") );
	line->setTag( Tags::NonPickable, true );
	line->addTransform();
	line->addComponent( geometry );
	
	return line;
}

//-----------------------------------//

RenderablePtr buildFrustum( const Frustum& box )
{
	const int numColors = 6*4; // Faces*Vertices
	std::vector<Vector3> colors( numColors, Color::White );

	VertexBufferPtr vb = new VertexBuffer();
	vb->set( VertexAttribute::Color, colors );

	MaterialPtr mat( new Material("FrustumDebug") );
	mat->setBackfaceCulling( false );

	RenderablePtr renderable = new Renderable(PolygonType::Quads);
	renderable->setVertexBuffer(vb);
	renderable->setMaterial(mat);
	renderable->setPolygonMode( PolygonMode::Wireframe );

	updateDebugFrustum(renderable, box);
	return renderable;
}

//-----------------------------------//

#define ADD_BOX_FRUSTUM( a, b, c, d )	\
	pos.push_back( box.corners[a] );	\
	pos.push_back( box.corners[b] );	\
	pos.push_back( box.corners[c] );	\
	pos.push_back( box.corners[d] );

void updateDebugFrustum( const RenderablePtr& rend, const Frustum& box )
{
	std::vector<Vector3> pos;
	ADD_BOX_FRUSTUM( 0, 2, 3, 1 ) // Front
	ADD_BOX_FRUSTUM( 0, 1, 5, 4 ) // Bottom
	ADD_BOX_FRUSTUM( 4, 5, 7, 6 ) // Back
	ADD_BOX_FRUSTUM( 2, 6, 7, 3 ) // Top
	ADD_BOX_FRUSTUM( 0, 4, 6, 2 ) // Left
	ADD_BOX_FRUSTUM( 1, 3, 7, 5 ) // Right

	VertexBufferPtr vb = rend->getVertexBuffer();
	vb->set( VertexAttribute::Position, pos );
}

//-----------------------------------//

} // end namespace