/****************************************************************************
 *  jaccount.h
 *
 *  Copyright (c) 2010 by Sidorov Aleksey <sauron@citadelspb.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*****************************************************************************/

#ifndef JACCOUNT_H
#define JACCOUNT_H

#include <qutim/account.h>
#include <jreen/presence.h>

namespace jreen
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
class JServiceDiscovery;
class JMUCManager;
class JSoftwareDetection;
class JVCardManager;
class JMessageSessionManager;

class JAccount : public Account
{
	Q_OBJECT
	Q_DECLARE_PRIVATE(JAccount)
public:
	JAccount(const QString &jid);
	virtual ~JAccount();
	ChatUnit *getUnitForSession(ChatUnit *unit);
	ChatUnit *getUnit(const QString &unitId, bool create = false);
	QString name() const;
	void setNick(const QString &nick);
	const QString &password(bool *ok = 0);
	jreen::Client *client() const;
	JSoftwareDetection *softwareDetection() const;
	JMessageSessionManager *messageSessionManager() const;
	JVCardManager *vCardManager() const;
	JRoster *roster() const;
	JServiceDiscovery *discoManager();
	JMUCManager *conferenceManager();
	jreen::PrivateXml *privateXml();
	jreen::PrivacyManager *privacyManager();
	jreen::PubSub::Manager *pubSubManager();
	virtual void setStatus(Status status);
	void setAccountStatus(Status status);
	QString getAvatarPath();
	void setAvatar(const QString &hex);
	bool event(QEvent *);
	QSet<QString> features() const;
	bool checkFeature(const QString &feature) const;
	bool checkIdentity(const QString &category, const QString &type) const;
	QString identity(const QString &category, const QString &type) const;
	void setPasswd(const QString &passwd);
public slots:
	void loadSettings();
private:
	//jreen
	friend class JRoster;
	friend class JRosterPrivate;
	friend class JServerDiscoInfo;
	QScopedPointer<JAccountPrivate> d_ptr;

	Q_PRIVATE_SLOT(d_func(),void _q_connected());
	Q_PRIVATE_SLOT(d_func(),void _q_disconnected());
	Q_PRIVATE_SLOT(d_func(),void _q_init_extensions(const QSet<QString> &features));
	Q_PRIVATE_SLOT(d_func(),void _q_on_module_loaded(int i));
};
} // Jabber namespace

#endif // JACCOUNT_H
