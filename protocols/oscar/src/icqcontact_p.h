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

#include "icqcontact.h"

#ifndef ICQCONTACT_PH_H
#define ICQCONTACT_PH_H

#include "capability.h"
#include "oscarconnection.h"
#include <QDateTime>
#include <QTimer>

namespace qutim_sdk_0_3 {

namespace oscar {

inline uint qHash(const QPointer<IcqContact> &ptr)
{ return qHash(ptr.data()); }

enum SsiBuddyTlvs
{
	SsiBuddyReqAuth = 0x0066,
	SsiBuddyProto   = 0x0084,
	SsiBuddyNick    = 0x0131,
	SsiBuddyComment = 0x013c,
	SsiBuddyTags    = 0x349c,
	SsiGroupDefault = 0x349d
};

enum ContactCapabilityFlags
{
	html_support      = 0x0001,
	utf8_support      = 0x0002,
	srvrelay_support  = 0x0004
};

class ChatStateUpdater : public QObject
{
	Q_OBJECT
public:
	ChatStateUpdater();
	void updateState(IcqContact *contact, ChatState state);
private slots:
	void sendState();
private:
	void sendState(IcqContact *contact, ChatState state);
	QHash<QPointer<IcqContact>, ChatState> m_states;
	QTimer m_timer;
};

class IcqContactPrivate
{
public:
	Q_DECLARE_PUBLIC(IcqContact)
	
	void clearCapabilities();
	void requestNick();
	void setCapabilities(const Capabilities &caps);
	FeedbagItem getNotInListGroup();
	Q_DECLARE_FLAGS(CapabilityFlags, ContactCapabilityFlags)
	IcqAccount *account;
	QString uin;
	QString name;
	QString proto;
	Status status;
	QString avatar;
	quint16 version;
	bool isInList;
	CapabilityFlags flags;
	Capabilities capabilities;
	DirectConnectionInfo dc_info;
	QStringList tags;
	ChatState state;
	QDateTime onlineSince;
	QDateTime awaySince;
	QDateTime regTime;
	IcqContact *q_ptr;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(IcqContactPrivate::CapabilityFlags)

} } // namespace qutim_sdk_0_3::oscar

#endif // ICQCONTACT_PH_H

