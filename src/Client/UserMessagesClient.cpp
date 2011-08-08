/************************************************************************
*
* vapor3D Server � (2008-2010)
*
*	<http://www.vapor3d.org>
*
************************************************************************/

#include "Editor/API.h"
#include "Protocol/UserMessages.h"
#include "Protocol/ReplicaMessages.h"
#include "Plugins/Networking/ServerPlugin.h"
#include "Network/Host.h"
#include "Network/Session.h"
#include "Editor.h"

NAMESPACE_EDITOR_BEGIN

//-----------------------------------//

class UserMessagesClient : UserMessagePlugin
{
	void handleUserAuthStatus(const SessionPtr&, const UserAuthStatusMessage&) OVERRIDE;
};

REFLECT_CHILD_CLASS(UserMessagesClient, UserMessagePlugin)
REFLECT_CLASS_END()

//-----------------------------------//

void UserMessagesClient::handleUserAuthStatus(const SessionPtr& session, const UserAuthStatusMessage&)
{
	ReplicaAskUpdateMessage ask;
	
	MessagePtr m_ask = MessageCreate(ReplicaMessageIds::ReplicaAskUpdate);
	m_ask->write(&ask);

	ServerPlugin* serverPlugin = GetPlugin<ServerPlugin>();
	serverPlugin->host->getPeer()->queueMessage(m_ask, 0);
}

//-----------------------------------//

NAMESPACE_EDITOR_END