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

#ifndef CHAT_H
#define CHAT_H

#include <QDeclarativeComponent>
#include "chatchannel.h"

namespace MeegoIntegration
{
class Chat : public qutim_sdk_0_3::ChatLayer
{
    Q_OBJECT
	Q_CLASSINFO("Uses", "IconLoader")
	Q_PROPERTY(qutim_sdk_0_3::ChatSession* activeSession READ activeSession WRITE setActiveSession NOTIFY activeSessionChanged)
	Q_PROPERTY(QDeclarativeListProperty<MeegoIntegration::ChatChannel> channels READ channels NOTIFY channelsChanged)
public:
    explicit Chat();
	virtual ~Chat();
	
	static void init();
	
	virtual qutim_sdk_0_3::ChatSession *getSession(qutim_sdk_0_3::ChatUnit *unit, bool create = true);
	virtual QList<qutim_sdk_0_3::ChatSession*> sessions();
	QDeclarativeListProperty<ChatChannel> channels();
	qutim_sdk_0_3::ChatSession *activeSession() const;
	void setActiveSession(qutim_sdk_0_3::ChatSession *session);
	void handleSessionDeath(qutim_sdk_0_3::ChatSession *session);
	Q_INVOKABLE void show();
	
signals:
	void sessionDestroyed(qutim_sdk_0_3::ChatSession *session);
	void activeSessionChanged(qutim_sdk_0_3::ChatSession *session);
	void channelsChanged(const QDeclarativeListProperty<ChatChannel> &channels);
	void shown();
	
private slots:
	void onSessionActivated(bool active);
	void onSessionDestroyed(QObject *object);
	
private:
	QList<ChatChannel*> m_channels;
	qutim_sdk_0_3::ChatSession *m_activeSession;
};
}

#endif // CHAT_H

