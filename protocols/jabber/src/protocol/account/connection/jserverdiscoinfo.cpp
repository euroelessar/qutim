/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
**
*****************************************************************************
**
** $QUTIM_BEGIN_LICENSE$
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
** See the GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see http://www.gnu.org/licenses/.
** $QUTIM_END_LICENSE$
**
****************************************************************************/
//#include "jserverdiscoinfo.h"
//#include <gloox/client.h>
//#include "protocol/account/jaccount_p.h"

//namespace Jabber {

//JServerDiscoInfo::JServerDiscoInfo(JAccount *account) :
//	m_account(account)
//{
//	//account->connection()->client()->registerConnectionListener(this);
//}

//JServerDiscoInfo::~JServerDiscoInfo()
//{
//}

//void JServerDiscoInfo::onConnect()
//{
//	//gloox::Client *client = m_account->connection()->client();
//	//JID server(client->jid().server());
//	//client->disco()->getDiscoInfo(server, "", this, ServerDiscoInfo);
//}

//void JServerDiscoInfo::onDisconnect(ConnectionError error)
//{
//	//Q_UNUSED(error);
//	//m_account->p->client.serverFeatures();
//	//m_account->p->identities.clear();
//}

//void JServerDiscoInfo::handleDiscoInfo(const JID &from, const Disco::Info &info, int context)
//{
//	Q_UNUSED(from);
//	Q_ASSERT(context == ServerDiscoInfo);

//	//		QSet<QString> features;
//	//		foreach (const std::string &feature, info.features())
//	//			features.insert(QString::fromStdString(feature));
//	//		m_account->p->features = features;


//	//		Identities identities;
//	//		foreach (Disco::Identity *identity, info.identities()) {
//	//			QString category = QString::fromStdString(identity->category());
//	//			QString type = QString::fromStdString(identity->type());
//	//			QString name = QString::fromStdString(identity->name());
//	//			Identities::iterator catItr = identities.find(category);
//	//			if (catItr == identities.end())
//	//				catItr = identities.insert(category, QHash<QString, QString>());
//	//			catItr->insert(type, name);
//	//		}
//	//		m_account->p->identities = identities;
//}

//void JServerDiscoInfo::handleDiscoItems(const JID &from, const Disco::Items &items, int context)
//{
//	Q_UNUSED(from);
//	Q_UNUSED(items);
//	Q_UNUSED(context);
//}

//void JServerDiscoInfo::handleDiscoError(const JID &from, const Error *error, int context)
//{
//	Q_UNUSED(from);
//	Q_UNUSED(error);
//	Q_UNUSED(context);
//}

//} // namespace Jabber

