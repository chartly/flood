/************************************************************************
*
* Flood Project � (2008-201x)
* Licensed under the simplified BSD license. All rights reserved.
*
************************************************************************/

#include "Engine/API.h"
#include "Engine/Scene/Group.h"
#include "Engine/Scene/Transform.h"
#include <algorithm>

NAMESPACE_ENGINE_BEGIN

//-----------------------------------//

REFLECT_CHILD_CLASS(Group, Entity)
	FIELD_VECTOR_PTR(6, Entity, EntityPtr, entities, RefPointer)
	FIELD_ALIAS(entities, nodes)
REFLECT_CLASS_END()

//-----------------------------------//

Group::Group()
{ }

//-----------------------------------//

Group::Group( const String& name )
	: Entity(name)
{ }

//-----------------------------------//

Group::~Group()
{
	for( size_t i = 0; i < entities.size(); i++ )
	{
		const EntityPtr& entity = entities[i];
		
		if( entity->references.read() != 1 )
            LogDebug("Entity '%s' should not have any references", entity->getName().c_str());
	}
}

//-----------------------------------//

void Group::add( const EntityPtr& entity )
{
	if( !entity ) return;

	entity->setParent( this );
	entities.pushBack( entity );

	onEntityAdded(entity);
	onEntityChanged();
}

//-----------------------------------//

EntityPtr Group::findEntity( const String& name ) const
{
	for( size_t i = 0; i < entities.size(); i++ )
	{
		const EntityPtr& entity = entities[i];
		
		if( entity->getName() == name )
			return entity;
	}

	return nullptr;
}

//-----------------------------------//

bool Group::remove( const EntityPtr& entity )
{
	auto it = std::find(entities.begin(), entities.end(), entity);

	if( it == entities.end() )
		return false;

	onEntityRemoved(entity);
	onEntityChanged();

	entities.remove(it);

	return true;
}

//-----------------------------------//

void Group::update( float delta )
{
	for( size_t i = 0; i < entities.size(); i++ )
	{
		const EntityPtr& entity = entities[i];
		if(entity) entity->update( delta );
	}
}

//-----------------------------------//

void Group::fixUp()
{
	Array<EntityPtr> invalid;

	for( size_t i = 0; i < entities.size(); i++ )
	{
		const EntityPtr& entity = entities[i];
		
		if( !entity )
		{
			invalid.pushBack(entity);
			continue;
		}

		if( !entity->getParent() ) entity->setParent(this);
	}

	// Remove invalid entities
	for( size_t i = 0; i < invalid.size(); i++ )
	{
		auto it = std::find(entities.begin(), entities.end(), invalid[i]);
		entities.remove(it);
	}
}

//-----------------------------------//

NAMESPACE_ENGINE_END