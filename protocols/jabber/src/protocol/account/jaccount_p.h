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
#ifndef JACCOUNT_P_H
#define JACCOUNT_P_H

#include "jaccount.h"
#include "muc/jbookmarkmanager.h"
#include <qutim/passworddialog.h>
//Jreen
#include <jreen/client.h>
#include <jreen/privacymanager.h>
//#include <jreen/pubsubmanager.h>
#include "../../sdk/jabber.h"
#include <QSignalMapper>
#include <QNetworkProxy>
#include <qutim/servicemanager.h>
#include <qutim/keychain.h>

namespace Jreen
{
class Client;
}

namespace Jabber {

typedef QHash<QString, QHash<QString, QString> > Identities;

class JAccountPrivate
{
	Q_DECLARE_PUBLIC(JAccount)
public:
	inline JAccountPrivate(JAccount *q) : q_ptr(q) {}
	inline ~JAccountPrivate() {}

	QString keyChainId() const
	{
		return QStringLiteral("jabber.") + q_func()->id();
	}

	//Jreen
	QScopedPointer<Jreen::Client> client;
	QNetworkProxy proxy;
	JSoftwareDetection *softwareDetection;
	JRoster *roster;
	Jreen::PrivacyManager *privacyManager;
	JMessageSessionManager *messageSessionManager;
	Jreen::PubSub::Manager *pubSubManager;
	Jreen::PrivateXml *privateXml;
	JMessageHandler *messageHandler;
	JAccount *q_ptr;
	QString nick;
	QString avatar;
//	QString pgpKeyId;
//	QString currentPGPKeyId;
	bool isOnline;
	QList<JabberExtension *> extensions;
	QPointer<JMUCManager> conferenceManager;
	QPointer<PasswordDialog> passwordDialog;
	ServicePointer<KeyChain> keyChain;
	QSignalMapper signalMapper;
	int loadedModules;
	int priority;
	
	void applyStatus(const Status &status);
	void setPresence(Jreen::Presence);
	void _q_set_nick(const QString &nick);
	void _q_disconnected(Jreen::Client::DisconnectReason);
	void _q_init_extensions(const QSet<QString> &features);
	void _q_on_module_loaded(int i);
	void _q_connected();
	void onPasswordReceived(const QString &password);

	//old code
	Identities identities;
};

}


#endif // JACCOUNT_P_H

