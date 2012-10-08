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
	bool htmlEnabled;
	QHash<quint64, Cookie> cookies;
	Capabilities caps;
	QHash<QString, Capability> typedCaps;
	OscarStatus lastStatus;
	QHash<QString, IcqContact *> contacts;
	QList<RosterPlugin*> rosterPlugins;
	QString passwd;
	QScopedPointer<ConnectingInfo> connectingInfo;
	QScopedPointer<MessageSender> messageSender;
};

} } // namespace qutim_sdk_0_3::oscar

#endif // ICQACCOUNT_P_H

