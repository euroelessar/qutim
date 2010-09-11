/****************************************************************************
 *  icqaccount_p.h
 *
 *  Copyright (c) 2010 by Nigmatullin Ruslan <euroelessar@gmail.com>
 *                        Prokhin Alexey <alexey.prokhin@yandex.ru>
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


#ifndef ICQACCOUNT_P_H
#define ICQACCOUNT_P_H

#include "icqaccount.h"
#include "messages_p.h"
#include <QTimer>
#include "buddypicture.h"
#include <qutim/passworddialog.h>

namespace qutim_sdk_0_3 {

namespace oscar {

class OscarConnection;

class PasswordValidator: public QValidator
{
	Q_OBJECT
public:
	explicit PasswordValidator(QObject *parent = 0);
	virtual State validate(QString &input, int &pos) const;
};

struct ConnectingInfo
{
	// At the end of connecting the variable contains all removed contacts.
	QHash<QString, IcqContact*> removedContacts;
	// Tags that were before connecting.
	QHash<IcqContact*, QStringList> oldTags;
	// Contacts that were created while connecting.
	QList<IcqContact*> createdContacts;
};

class IcqAccountPrivate
{
public:
	Q_DECLARE_PUBLIC(IcqAccount);
	void loadRoster();
	void setCapability(const Capability &capability, const QString &type);
	bool removeCapability(const Capability &capability);
	bool removeCapability(const QString &type);
	QString password();
	IcqAccount *q_ptr;
	OscarConnection *conn;
	Feedbag *feedbag;
	BuddyPicture *buddyPicture;
	QString name;
	QString avatar;
	QHash<quint64, Cookie*> cookies;
	Capabilities caps;
	QHash<QString, Capability> typedCaps;
	OscarStatus lastStatus;
	QTimer reconnectTimer;
	QHash<QString, IcqContact *> contacts;
	QList<RosterPlugin*> rosterPlugins;
	QString passwd;
	QScopedPointer<ConnectingInfo> connectingInfo;
};

} } // namespace qutim_sdk_0_3::oscar

#endif // ICQACCOUNT_P_H
