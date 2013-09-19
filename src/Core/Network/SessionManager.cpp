/************************************************************************
*
* Flood Project © (2008-201x)
* Licensed under the simplified BSD license. All rights reserved.
*
************************************************************************/

#include "Core/API.h"
#include "Core/Network/SessionManager.h"
#include "Core/Network/Peer.h"

NAMESPACE_CORE_BEGIN

//-----------------------------------//

SessionManager::SessionManager()
{
}

//-----------------------------------//

SessionManager::~SessionManager()
{
}

//-----------------------------------//

void SessionManager::addSession(const SessionPtr& session)
{
	if( !session ) return;

	auto& hash = *session->getHash();
    uint64 key = 0;
    memcpy(&key, &hash[0], hash.size());
	
    assert(sessions.has(key));

	sessions.set(key, session);
	onSessionAdded(session);
}

//-----------------------------------//

void SessionManager::removeSession(const SessionPtr& session)
{
	if( !session ) return;

    auto& hash = *session->getHash();
    uint64 key = 0;
    memcpy(&key, &hash[0], hash.size());

	sessions.remove(key);
	onSessionRemoved(session);
}

//-----------------------------------//

Session* SessionManager::getSession(const SessionHash& hash) const
{
    uint64 key = 0;
    memcpy(&key, &hash[0], hash.size());
    
    auto sesh = sessions.get(key, nullptr);
    return sesh.get();
}

NAMESPACE_CORE_END