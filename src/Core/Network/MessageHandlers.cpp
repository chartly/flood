/************************************************************************
*
* Flood Project � (2008-201x)
* Licensed under the simplified BSD license. All rights reserved.
*
************************************************************************/

#include "Core/API.h"
#include "Core/Network/MessageHandlers.h"
#include "Core/Containers/Array.h"
#include "Core/Log.h"

NAMESPACE_CORE_BEGIN

//-----------------------------------//

void MessageHandlers::addHandlers(MessageHandler* handler, MessageDirection::Enum direction)
{
	const MessagesTable& messagesTable = handler->getMessagesTable();
	Enum* messagesEnum = handler->getMessagesEnum();

	for(size_t i = 0; i < array::size(messagesTable); ++i )
	{
		MessageMapping mapping = messagesTable[i];

		bool isBoth = mapping.direction == MessageDirection::Both;
		if(!(isBoth || mapping.direction == direction))
			continue;

		mapping.handler = handler;
		addMapping(mapping);

		const char* name = EnumGetValueName(messagesEnum, mapping.id);
		LogDebug("Registered message handler: %s", name);
	}
}

//-----------------------------------//

void MessageHandlers::removeHandlers(MessageHandler* handler)
{
	const MessagesTable& messagesTable = handler->getMessagesTable();
	Enum* messagesEnum = handler->getMessagesEnum();

	for(size_t i = 0; i < array::size(messagesTable); ++i)
	{
		const MessageMapping& mapping = messagesTable[i];
		removeMapping(mapping);
	}
}

//-----------------------------------//

MessageMapping* MessageHandlers::findHandler(MessageId id)
{
	MessageHandlersMap::iterator it = handlers.find(id);

	if( it == handlers.end() )
		return nullptr;

	return &it->second;
}

//-----------------------------------//

void MessageHandlers::addMapping(const MessageMapping& handler)
{
	MessageId id = handler.id;
	handlers[id] = handler;
}

//-----------------------------------//

void MessageHandlers::removeMapping(const MessageMapping& handler)
{
	MessageHandlersMap::iterator it = handlers.find(handler.id);

	if( it == handlers.end() )
		return;

	handlers.erase(it);
}

//-----------------------------------//

NAMESPACE_CORE_END