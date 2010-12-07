/************************************************************************
*
* vapor3D Engine � (2008-2010)
*
*	<http://www.vapor3d.org>
*
************************************************************************/

#include "vapor/PCH.h"

#ifdef VAPOR_PHYSICS_BULLET

#include "vapor/physics/MeshShape.h"
#include "vapor/physics/Convert.h"
#include "vapor/scene/Entity.h"
#include "vapor/scene/Transform.h"
#include "vapor/scene/Geometry.h"

#include <BulletCollision/CollisionShapes/btTriangleMesh.h>
#include <BulletCollision/CollisionShapes/btTriangleIndexVertexArray.h>
#include <BulletCollision/CollisionShapes/btBvhTriangleMeshShape.h>
#include <BulletCollision/Gimpact/btGImpactShape.h>

namespace vapor {

//-----------------------------------//

BEGIN_CLASS_PARENT(MeshShape, Shape)
END_CLASS()

//-----------------------------------//

MeshShape::MeshShape()
	: meshShape(nullptr)
{ }

//-----------------------------------//

MeshShape::~MeshShape()
{
	removeBody();
	delete meshShape;
}

//-----------------------------------//

//btTriangleIndexVertexArray* MeshShape::convertMesh()
//{
//	btTriangleIndexVertexArray* vertexArray;
//	vertexArray = new btTriangleIndexVertexArray();
//
//	const EntityPtr& node = getEntity();
//	
//	foreach( const GeometryPtr& geometry, node->getGeometry() )
//	{
//		const RenderableList& rends = geometry->getRenderables();
//
//		foreach( const RenderablePtr& rend, rends )
//		{
//			if( rend->getPrimitiveType() != PolygonType::Triangles )
//				continue;
//
//			VertexBufferPtr vb = rend->getVertexBuffer();
//			const std::vector<Vector3>& pos = vb->getAttribute( VertexAttribute::Position );
//			
//			IndexBufferPtr ib = rend->getIndexBuffer();
//			const std::vector<ushort>& indices = ib->getIndices16();
//
// 		//	btIndexedMesh* indexMesh = ;
//
//			//foreach( const Vector3& vec, pos )
//			//{
//			//	//indexMesh.m_numTriangles
//			//}
//
//			//vertexArray->addIndexedMesh(indexMesh);
//		}
//	}
//
//	return vertexArray;
//}

btTriangleMesh* MeshShape::convertMesh()
{
	btTriangleMesh* mesh = new btTriangleMesh(false);

	const EntityPtr& node = getEntity();
	
	for( uint i = 0; i < node->getGeometry().size(); i++ )
	{
		const GeometryPtr& geometry = node->getGeometry()[i];
		const RenderableList& rends = geometry->getRenderables();

		for( uint e = 0; e < rends.size(); e++ )
		{
			const RenderablePtr& rend = rends[e];

			if( rend->getPrimitiveType() != PolygonType::Triangles )
				continue;

			VertexBufferPtr vb = rend->getVertexBuffer();
			const std::vector<Vector3>& pos = vb->getAttribute( VertexAttribute::Position );
			
			IndexBufferPtr ib = rend->getIndexBuffer();

			if( !ib )
			{		
				for( uint i = 0; i < pos.size(); i += 3 )
				{
					btVector3 vertex0 = Convert::toBullet(pos[i]);
					btVector3 vertex1 = Convert::toBullet(pos[i+1]);
					btVector3 vertex2 = Convert::toBullet(pos[i+2]);
					
					mesh->addTriangle(vertex0, vertex1, vertex2);
				}
			}
			else
			{
				const std::vector<ushort>& ind = ib->getIndices16();

				for( uint i = 0; i < ind.size(); i += 3 )
				{
					btVector3 vertex0 = Convert::toBullet(pos[ind[i]]);
					btVector3 vertex1 = Convert::toBullet(pos[ind[i+1]]);
					btVector3 vertex2 = Convert::toBullet(pos[ind[i+2]]);
					
					mesh->addTriangle(vertex0, vertex1, vertex2);
				}
			}
		}
	}

	return mesh;
}

//-----------------------------------//

void MeshShape::update( double delta )
{
	if( meshShape )
		return;	

	meshShape = new btBvhTriangleMeshShape( convertMesh(), true );
	//meshShape->updateBound();
	
	const TransformPtr& transform = getEntity()->getTransform();
	const Vector3& scale = transform->getScale();
	meshShape->setLocalScaling(Convert::toBullet(scale));
}

//-----------------------------------//

btCollisionShape* const MeshShape::getBulletShape() const
{
	return meshShape;
}

//-----------------------------------//

} // end namespace

#endif