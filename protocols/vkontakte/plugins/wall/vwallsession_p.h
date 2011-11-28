/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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
#ifndef VWALLSESSION_P_H
#define VWALLSESSION_P_H
#include <QObject>
#include <QTimer>
#include <QVariantMap>

namespace qutim_sdk_0_3 {
	class Buddy;
	class Contact;
	class Message;
}

class VAccount;
class VContact;
class VWallSession;
class VWallSessionPrivate : public QObject
{
	Q_DECLARE_PUBLIC(VWallSession)
	Q_OBJECT
public:
    VWallSessionPrivate() : 
		QObject(0),q_ptr(0),id(QString()),me(0),owner(0),historyCount(0),timeStamp(0)
	{
		
	}
    virtual ~VWallSessionPrivate() {}
    VWallSession *q_ptr;
	QString id;
	VAccount *account();
	VContact *me;
	VContact *owner;
	int historyCount;
	QTimer updateTimer;
	int timeStamp;
	void processMultimediaMessage(qutim_sdk_0_3::Message &mess, const QVariantMap &data);
public slots:
	void getHistory();
	void onGetHistoryFinished();
};

#endif // VWALLSESSION_P_H

