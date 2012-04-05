/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Aleksey Sidorov <gorthauer87@yandex.ru>
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

#ifndef JACCOUNT_H
#define JACCOUNT_H

#include <qutim/account.h>
#include <jreen/presence.h>

namespace Jreen
{
class Client;
class JID;
class PrivateXml;
class PrivacyManager;
namespace PubSub { class Manager; }
}

namespace Jabber {

using namespace qutim_sdk_0_3;
//using namespace gloox;

class JAccountPrivate;
class JRoster;
class JRosterPrivate;
class JConnection;
class JMessageHandler;
class JMUCManager;
class JSoftwareDetection;
class JVCardManager;
class JMessageSessionManager;

class JAccount : public Account
{
	Q_OBJECT
	Q_PROPERTY(QObject* client READ client)
	Q_PROPERTY(QObject* privateXml READ privateXml)
	Q_PROPERTY(QObject* privacyManager READ privacyManager)
	Q_PROPERTY(QObject* pubSubManager READ pubSubManager)
	Q_DECLARE_PRIVATE(JAccount)
	Q_PROPERTY(QString avatar READ avatar NOTIFY avatarChanged)
public:
	JAccount(const QString &jid);
	virtual ~JAccount();
	ChatUnit *getUnitForSession(ChatUnit *unit);
	ChatUnit *getUnit(const QString &unitId, bool create = false);
	QString name() const;
	QString getPassword() const;
	Jreen::Client *client() const;
	JSoftwareDetection *softwareDetection() const;
	JMessageSessionManager *messageSessionManager() const;
	JRoster *roster() const;
	JMUCManager *conferenceManager();
	Jreen::PrivateXml *privateXml();
	Jreen::PrivacyManager *privacyManager();
	Jreen::PubSub::Manager *pubSubManager();
	virtual void setStatus(Status status);
	void setAccountStatus(Status status);
	QString getAvatarPath();
	void setAvatarHex(const QString &hex);
	QString avatar();
	bool event(QEvent *);
	QSet<QString> features() const;
	bool checkFeature(const QString &feature) const;
	bool checkIdentity(const QString &category, const QString &type) const;
	QString identity(const QString &category, const QString &type) const;
	void setPasswd(const QString &passwd);
	
	QStringList updateParameters(const QVariantMap &parameters, bool forced = false);
	void loadParameters();
	QString pgpKeyId() const;
	
protected:
	virtual void virtual_hook(int id, void *data);
signals:
	void avatarChanged(const QString &avatar);
	void pgpKeyIdChanged(QString pgpKeyId);
	
public slots:
	void loadSettings();

private:
	//Jreen
	friend class JRoster;
	friend class JRosterPrivate;
	friend class JServerDiscoInfo;
	QScopedPointer<JAccountPrivate> d_ptr;
	
	Q_PRIVATE_SLOT(d_func(),void _q_set_nick(const QString &nick))
	Q_PRIVATE_SLOT(d_func(),void _q_connected())
	Q_PRIVATE_SLOT(d_func(),void _q_disconnected(Jreen::Client::DisconnectReason))
	Q_PRIVATE_SLOT(d_func(),void _q_init_extensions(const QSet<QString> &features))
	Q_PRIVATE_SLOT(d_func(),void _q_on_module_loaded(int i))
	Q_PRIVATE_SLOT(d_func(),void _q_on_password_finished(int))
};
} // Jabber namespace

#endif // JACCOUNT_H

