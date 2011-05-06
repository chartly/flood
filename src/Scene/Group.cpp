/************************************************************************
*
* vapor3D Engine � (2008-2010)
*
*	<http://www.vapor3d.org>
*
************************************************************************/

#include "Engine/API.h"
#include "Scene/Group.h"
#include <algorithm>

namespace vapor {

//-----------------------------------//

REFLECT_CHILD_CLASS(Group, Entity)
	FIELD_VECTOR_PTR(Entity, EntityPtr, entities, SharedPointer)
REFLECT_CLASS_END()

//-----------------------------------//

Group::Group()
{ }

//-----------------------------------//

Group::Group( const std::string& name )
	: Entity(name)
{ }

//-----------------------------------//

void Group::add( const EntityPtr& entity )
{
	// Beware that you have to assign a new Group-derived object
	// to a shared_ptr, else shared_from_this will return bad_weak_ptr.

	if( !entity ) return;

	entity->setParent( this );
	entities.push_back( entity );

	onEntityAdded(entity);
	onEntityChanged();
}

//-----------------------------------//

EntityPtr Group::findEntity( const std::string& name ) const
{
	for( size_t i = 0; i < entities.size(); i++ )
	{
		const EntityPtr& entity = entities[i];
		
		if( entity->getName() == name )
			return entity;
	}

	return EntityPtr();
}

//-----------------------------------//

bool Group::remove( const EntityPtr& entity )
{
	std::vector<EntityPtr>::iterator it;
	it = std::find(entities.begin(), entities.end(), entity);

	if( it == entities.end() )
		return false;

	onEntityRemoved(entity);
	onEntityChanged();

	entities.erase(it);

	return true;
}

//-----------------------------------//

void Group::update( float delta )
{
	for( size_t i = 0; i < entities.size(); i++ )
	{
		const EntityPtr& entity = entities[i];
		entity->update( delta );
	}
}

//-----------------------------------//

} // end namespace