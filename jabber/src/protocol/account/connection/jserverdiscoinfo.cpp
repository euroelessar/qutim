#include "jserverdiscoinfo.h"
#include <gloox/client.h>
#include "protocol/account/jaccount_p.h"

namespace Jabber {

	JServerDiscoInfo::JServerDiscoInfo(JAccount *account) :
		m_account(account)
	{
		account->connection()->client()->registerConnectionListener(this);
	}

	JServerDiscoInfo::~JServerDiscoInfo()
	{
	}

	void JServerDiscoInfo::onConnect()
	{
		gloox::Client *client = m_account->connection()->client();
		JID server(client->jid().server());
		client->disco()->getDiscoInfo(server, "", this, ServerDiscoInfo);
	}

	void JServerDiscoInfo::onDisconnect(ConnectionError error)
	{
		Q_UNUSED(error);
		m_account->p->client.serverFeatures();
		m_account->p->identities.clear();
	}

	void JServerDiscoInfo::handleDiscoInfo(const JID &from, const Disco::Info &info, int context)
	{
		Q_UNUSED(from);
		Q_ASSERT(context == ServerDiscoInfo);

//		QSet<QString> features;
//		foreach (const std::string &feature, info.features())
//			features.insert(QString::fromStdString(feature));
//		m_account->p->features = features;


//		Identities identities;
//		foreach (Disco::Identity *identity, info.identities()) {
//			QString category = QString::fromStdString(identity->category());
//			QString type = QString::fromStdString(identity->type());
//			QString name = QString::fromStdString(identity->name());
//			Identities::iterator catItr = identities.find(category);
//			if (catItr == identities.end())
//				catItr = identities.insert(category, QHash<QString, QString>());
//			catItr->insert(type, name);
//		}
//		m_account->p->identities = identities;
	}

	void JServerDiscoInfo::handleDiscoItems(const JID &from, const Disco::Items &items, int context)
	{
		Q_UNUSED(from);
		Q_UNUSED(items);
		Q_UNUSED(context);
	}

	void JServerDiscoInfo::handleDiscoError(const JID &from, const Error *error, int context)
	{
		Q_UNUSED(from);
		Q_UNUSED(error);
		Q_UNUSED(context);
	}

} // namespace Jabber
