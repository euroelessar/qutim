/****************************************************************************
 *  event.h
 *
 *  Copyright (c) 2010 by Nigmatullin Ruslan <euroelessar@gmail.com>
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

#ifndef EVENT_H
#define EVENT_H

#include "libqutim_global.h"
#include <QtCore/QVariant>
#include <QtCore/QVarLengthArray>
#include <QtCore/QObject>
#include <QtCore/QEvent>

namespace qutim_sdk_0_3
{
	class LIBQUTIM_EXPORT Event : public QEvent
	{
	public:
		inline Event(const char *id = 0, const QVariant &arg0 = QVariant(),
					 const QVariant &arg1 = QVariant(), const QVariant &arg2 = QVariant(),
					 const QVariant &arg3 = QVariant(), const QVariant &arg4 = QVariant());

		inline Event(quint16 id, const QVariant &arg0 = QVariant(),
					 const QVariant &arg1 = QVariant(), const QVariant &arg2 = QVariant(),
					 const QVariant &arg3 = QVariant(), const QVariant &arg4 = QVariant());

		static QEvent::Type eventType();
		static quint16 registerType(const char *id);
		static const char *getId(quint16 id);

		template<typename T>
		T at(int index) const { args[index].value<T>(); }
		void send();

		quint16 id;
		QVarLengthArray<QVariant, 5> args;
	};

	Event::Event(const char *id, const QVariant &arg0, const QVariant &arg1, const QVariant &arg2,
		  const QVariant &arg3, const QVariant &arg4) : QEvent(eventType()), args(5)
	{
		this->id = registerType(id);
		args[0] = arg0; args[1] = arg1; args[2] = arg2; args[3] = arg3; args[4] = arg4;
	}

	Event::Event(quint16 id, const QVariant &arg0, const QVariant &arg1, const QVariant &arg2,
		  const QVariant &arg3, const QVariant &arg4) : QEvent(eventType()), args(10)
	{
		this->id = id;
		args[0] = arg0; args[1] = arg1; args[2] = arg2; args[3] = arg3; args[4] = arg4;
	}

	LIBQUTIM_EXPORT QDebug operator<<(QDebug, const Event &);
}

#endif // EVENT_H
