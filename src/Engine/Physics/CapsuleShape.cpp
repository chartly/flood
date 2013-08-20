/************************************************************************
*
* Flood Project © (2008-201x)
* Licensed under the simplified BSD license. All rights reserved.
*
************************************************************************/

#include "Engine/API.h"
#include "Core/Event.h"

#ifdef ENABLE_PHYSICS_BULLET

#include "Engine/Physics/CapsuleShape.h"
#include "Engine/Physics/Convert.h"
#include "Engine/Physics/Body.h"

#include "Engine/Scene/Entity.h"
#include "Engine/Scene/Transform.h"

#include <BulletCollision/CollisionShapes/btCapsuleShape.h>

NAMESPACE_ENGINE_BEGIN

//-----------------------------------//

REFLECT_CHILD_CLASS(CapsuleShape, Shape)
	FIELD_PRIMITIVE(1, float, height)
	FIELD_PRIMITIVE(2, float, radius)
REFLECT_CLASS_END()

//-----------------------------------//

CapsuleShape::CapsuleShape()
	: capsuleShape(nullptr)
	, radius(1.0f)
	, height(3.0f)
{
}

//-----------------------------------//

CapsuleShape::~CapsuleShape()
{
	removeBody();
	Deallocate(capsuleShape);
}

//-----------------------------------//

void CapsuleShape::update( float delta )
{
	if( capsuleShape ) return;

	const TransformPtr& transform = getEntity()->getTransform();
	if( !transform ) return;
	
	const Vector3& scale = transform->getScale();

	capsuleShape = AllocateThis(btCapsuleShape, radius, height);
	if(capsuleShape)
		capsuleShape->setLocalScaling(Convert::toBullet(scale));
	else
		LogError("Failed to allocate btCapsuleShape while updating CapsuleShape.");
}

//-----------------------------------//

btCollisionShape* const CapsuleShape::getBulletShape() const
{
	return capsuleShape;
}

//-----------------------------------//

void CapsuleShape::onFieldChanged(const Field& field)
{
	if( !capsuleShape )
		return;

	const BodyPtr& body = getEntity()->getComponent<Body>();

	if( body )
		body->removeWorld();

	btVector3 dimensions(radius, 0.5*height, radius);
	capsuleShape->setImplicitShapeDimensions(dimensions);

	if( body )
		body->addWorld();
}

//-----------------------------------//

NAMESPACE_ENGINE_END

#endif