/*
   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef NOTIFYHELPER_H
#define NOTIFYHELPER_H

#include <QObject>
#include <qutim/chatunit.h>
#include <knotification.h>


using namespace qutim_sdk_0_3;

class NotifyHelper : public QObject
{
	Q_OBJECT
public:
	NotifyHelper (ChatUnit *unit, const QVariant &data);
	void registerNotification (KNotification*);
	~NotifyHelper();
private: 	
	QPointer<ChatUnit> m_unit;
	KNotification *m_notification;
public slots:
 	void startChatSlot (); //открываем окно чата
	void closeSlot(); //закрываем окно уведомления
};

#endif // NOTIFYHELPER_H
