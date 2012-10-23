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
class Chat : public Ureen::ChatLayer
{
    Q_OBJECT
	Q_CLASSINFO("Uses", "IconLoader")
	Q_PROPERTY(Ureen::ChatSession* activeSession READ activeSession WRITE setActiveSession NOTIFY activeSessionChanged)
	Q_PROPERTY(QDeclarativeListProperty<MeegoIntegration::ChatChannel> channels READ channels NOTIFY channelsChanged)
public:
    explicit Chat();
	virtual ~Chat();
	
	static void init();
	
	virtual Ureen::ChatSession *getSession(Ureen::ChatUnit *unit, bool create = true);
	virtual QList<Ureen::ChatSession*> sessions();
	QDeclarativeListProperty<ChatChannel> channels();
	Ureen::ChatSession *activeSession() const;
	void setActiveSession(Ureen::ChatSession *session);
	void handleSessionDeath(Ureen::ChatSession *session);
	Q_INVOKABLE void show();
	
signals:
	void sessionDestroyed(Ureen::ChatSession *session);
	void activeSessionChanged(Ureen::ChatSession *session);
	void channelsChanged(const QDeclarativeListProperty<ChatChannel> &channels);
	void shown();
	
private slots:
	void onSessionActivated(bool active);
	void onSessionDestroyed(QObject *object);
	
private:
	QList<ChatChannel*> m_channels;
	Ureen::ChatSession *m_activeSession;
};
}

#endif // CHAT_H

