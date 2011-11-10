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

#include "event.h"
#include <QtCore/QPointer>
#include <QtCore/QDebug>
#include <QtCore/QSharedData>
#include <QtCore/QCoreApplication>

namespace qutim_sdk_0_3
{
	Q_GLOBAL_STATIC(QObject, eventManagerHandler)

	void Event::send()
	{
		QCoreApplication::sendEvent(eventManagerHandler(), this);
	}

	static QVector<QByteArray> event_ids;

	QEvent::Type Event::eventType()
	{
		static QEvent::Type type = QEvent::Type(QEvent::registerEventType(QEvent::User + 100));
		return type;
	}

	quint16 Event::registerType(const char *id)
	{
		if(!id)
			return 0xffff;
		for(int i = 0; i < event_ids.size(); i++)
			if(event_ids[i] == id)
				return i;
		event_ids.append(id);
		return event_ids.size() - 1;
	}

	const char *Event::getId(quint16 id)
	{
		if(id >= event_ids.size())
			return 0;
		return event_ids[id];
	}

	QObject *Event::eventManager()
	{
		return eventManagerHandler();
	}

	QDebug operator<<(QDebug dbg, const Event &e)
	{
		QVariantList list;
		for(int i = 0; i < e.args.size() && !e.args[i].isNull(); i++)
			list << e.args[i];
		dbg.nospace() << "(\"" << Event::getId(e.id) << "\": ";
		if(!e.args[0].isNull())
			dbg << e.args[0];
		for(int i = 1; i < e.args.size() && !e.args[i].isNull(); i++)
			dbg << ", " << e.args[i];
		return dbg << " ) ";
	}
}

