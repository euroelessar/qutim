/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Rusanov Peter <peter.rusanov@gmail.com>
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

#ifndef ROSTER_H
#define ROSTER_H

#include <QMultiMap>
#include <QVariantList>

#include "mrimcontact.h"
#include "protoutils.h"

#include "mrimpacket.h"

class MrimRosterResult : public QVariantList
{
public:
	QString getString(int i, bool unicode)
	{
		return at(i).value<LPString>().toString(unicode);
	}
	quint32 getUInt(int i)
	{
		return at(i).toUInt();
	}
};

class MrimRoster : public QObject, public PacketHandler
{
    Q_OBJECT
public:
    MrimRoster(class MrimAccount* acc);
    virtual ~MrimRoster();
    virtual QList<quint32> handledTypes();
    virtual bool handlePacket(MrimPacket& packet);
    QString groupName(quint32 groupId) const;
    MrimContact *getContact(const QString& id, bool create);

protected:
    void addToList(class MrimContact *cnt);
	void handleUserInfo(MrimPacket &packet);
	void handleAuthorizeAck(MrimPacket &packet);
    bool parseList(MrimPacket& packet);    
    bool parseGroups(MrimPacket& packet, quint32 count, const QString& mask);
    bool parseContacts(MrimPacket& packet, const QString& mask);    
    MrimRosterResult parseByMask(MrimPacket& packet, const QString& mask);
    bool handleStatusChanged(MrimPacket &packet);

public slots:
    void handleLoggedOut();

private:
    Q_DISABLE_COPY(MrimRoster);
    QScopedPointer<class MrimRosterPrivate> p;
};

#endif // ROSTER_H

