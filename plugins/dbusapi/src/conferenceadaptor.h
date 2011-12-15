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

#ifndef CONFERENCEADAPTOR_H
#define CONFERENCEADAPTOR_H

#include <QDBusAbstractAdaptor>
#include <QDBusObjectPath>
#include <QDBusConnection>
#include <qutim/conference.h>

using namespace qutim_sdk_0_3;

class ConferenceAdaptor : public QDBusAbstractAdaptor
{
	Q_OBJECT
	Q_CLASSINFO("D-Bus Interface", "org.qutim.Conference")
	Q_PROPERTY(QString topic READ topic WRITE setTopic NOTIFY topicChanged)
	Q_PROPERTY(QDBusObjectPath me READ me NOTIFY meChanged)
public:
    explicit ConferenceAdaptor(const QDBusConnection &dbus, Conference *conf);
	inline QString topic() const { return static_cast<Conference*>(parent())->topic(); }
	inline void setTopic(const QString &text) const;
	QDBusObjectPath me() const;
	
public slots:
	inline void join() { static_cast<Conference*>(parent())->join(); }
	inline void leave() { static_cast<Conference*>(parent())->leave(); }
	
signals:
	void topicChanged(const QString &);
	void meChanged(const QDBusObjectPath &);
	void leaved();
	void joined();
	
private slots:
	void onMeChanged(qutim_sdk_0_3::Buddy *buddy);
private:
	QDBusConnection m_dbus;
};

void ConferenceAdaptor::setTopic(const QString &text) const
{
	return static_cast<Conference*>(parent())->setTopic(text);
}

#endif // CONFERENCEADAPTOR_H

