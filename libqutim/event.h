/****************************************************************************
 *  event.h
 *
 *  Copyright (c) 2009 by Nigmatullin Ruslan <euroelessar@gmail.com>
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

namespace qutim_sdk_0_3
{
	class LIBQUTIM_EXPORT Event
	{
	public:
		enum Priority
		{
			HighestPriority	= 0x4000,
			HighPriority	= 0x2000,
			NormalPriority	= 0x1000,
			LowPriority		= 0x0800,
			LowestPriority	= 0x0400
		};

		inline Event(const char *id = 0, const QVariant &arg0 = QVariant(), const QVariant &arg1 = QVariant(),
			  const QVariant &arg2 = QVariant(), const QVariant &arg3 = QVariant(),
			  const QVariant &arg4 = QVariant(), const QVariant &arg5 = QVariant(),
			  const QVariant &arg6 = QVariant(), const QVariant &arg7 = QVariant(),
			  const QVariant &arg8 = QVariant(), const QVariant &arg9 = QVariant());

		inline Event(quint16 id = 0, const QVariant &arg0 = QVariant(), const QVariant &arg1 = QVariant(),
			  const QVariant &arg2 = QVariant(), const QVariant &arg3 = QVariant(),
			  const QVariant &arg4 = QVariant(), const QVariant &arg5 = QVariant(),
			  const QVariant &arg6 = QVariant(), const QVariant &arg7 = QVariant(),
			  const QVariant &arg8 = QVariant(), const QVariant &arg9 = QVariant());

		inline Event(const char *id, const QVariant &arg0, const QVariant &arg1,
			  const QVariant &arg2, const QVariant &arg3,
			  const QVariant &arg4, const QVariant &arg5,
			  const QVariant &arg6, const QVariant &arg7,
			  const QVariant &arg8, const QVariant &arg9,
			  const QVariant &arg10 = QVariant(), const QVariant &arg11 = QVariant(),
			  const QVariant &arg12 = QVariant(), const QVariant &arg13 = QVariant(),
			  const QVariant &arg14 = QVariant(), const QVariant &arg15 = QVariant(),
			  const QVariant &arg16 = QVariant(), const QVariant &arg17 = QVariant(),
			  const QVariant &arg18 = QVariant(), const QVariant &arg19 = QVariant());

		inline Event(quint16 id, const QVariant &arg0, const QVariant &arg1,
			  const QVariant &arg2, const QVariant &arg3,
			  const QVariant &arg4, const QVariant &arg5,
			  const QVariant &arg6, const QVariant &arg7,
			  const QVariant &arg8, const QVariant &arg9,
			  const QVariant &arg10 = QVariant(), const QVariant &arg11 = QVariant(),
			  const QVariant &arg12 = QVariant(), const QVariant &arg13 = QVariant(),
			  const QVariant &arg14 = QVariant(), const QVariant &arg15 = QVariant(),
			  const QVariant &arg16 = QVariant(), const QVariant &arg17 = QVariant(),
			  const QVariant &arg18 = QVariant(), const QVariant &arg19 = QVariant());

		template<typename T>
		T at(int index) const { args[index].value<T>(); }
		inline void send();

		quint16 id;
		QVariant return_value;
		const QVarLengthArray<QVariant, 10> args;
	};


	class LIBQUTIM_EXPORT EventManager
	{
	public:
		typedef void (QObject::*Method)( Event &event );
		static quint16 eventId(const char *id);
		static const char *eventId(quint16 id);
		template<typename TMethod>
		static quint16 registerEventHandler(const char *id, QObject *obj, TMethod method, quint16 priority = Event::NormalPriority);
		static quint16 send(Event &event);
	private:
		static quint16 register_helper(QObject *obj, Method method, const char *id, quint16 priority);
	};

	Event::Event(const char *id, const QVariant &arg0, const QVariant &arg1, const QVariant &arg2,
		  const QVariant &arg3, const QVariant &arg4, const QVariant &arg5,
		  const QVariant &arg6, const QVariant &arg7, const QVariant &arg8, const QVariant &arg9) : args(10)
	{
		this->id = EventManager::eventId(id);
		QVarLengthArray<QVariant, 10> &args = *const_cast<QVarLengthArray<QVariant, 10>*>(&this->args);
		args[0] = arg0; args[1] = arg1; args[2] = arg2; args[3] = arg3; args[4] = arg4;
		args[5] = arg5; args[6] = arg6; args[7] = arg7; args[8] = arg8; args[9] = arg9;
	}

	Event::Event(quint16 id, const QVariant &arg0, const QVariant &arg1, const QVariant &arg2,
		  const QVariant &arg3, const QVariant &arg4, const QVariant &arg5,
		  const QVariant &arg6, const QVariant &arg7, const QVariant &arg8, const QVariant &arg9) : args(10)
	{
		this->id = id;
		QVarLengthArray<QVariant, 10> &args = *const_cast<QVarLengthArray<QVariant, 10>*>(&this->args);
		args[0] = arg0; args[1] = arg1; args[2] = arg2; args[3] = arg3; args[4] = arg4;
		args[5] = arg5; args[6] = arg6; args[7] = arg7; args[8] = arg8; args[9] = arg9;
	}

	Event::Event(const char *id, const QVariant &arg0, const QVariant &arg1, const QVariant &arg2, const QVariant &arg3,
		  const QVariant &arg4, const QVariant &arg5, const QVariant &arg6, const QVariant &arg7,
		  const QVariant &arg8, const QVariant &arg9, const QVariant &arg10, const QVariant &arg11,
		  const QVariant &arg12, const QVariant &arg13, const QVariant &arg14, const QVariant &arg15,
		  const QVariant &arg16, const QVariant &arg17, const QVariant &arg18, const QVariant &arg19) : args(20)
	{
		this->id = EventManager::eventId(id);
		QVarLengthArray<QVariant, 10> &args = *const_cast<QVarLengthArray<QVariant, 10>*>(&this->args);
		args[0] = arg0; args[1] = arg1; args[2] = arg2; args[3] = arg3; args[4] = arg4;
		args[5] = arg5; args[6] = arg6; args[7] = arg7; args[8] = arg8; args[9] = arg9;
		args[10] = arg10; args[11] = arg11; args[12] = arg12; args[13] = arg13; args[14] = arg14;
		args[15] = arg15; args[16] = arg16; args[17] = arg17; args[18] = arg18; args[19] = arg19;
	}

	Event::Event(quint16 id, const QVariant &arg0, const QVariant &arg1, const QVariant &arg2, const QVariant &arg3,
		  const QVariant &arg4, const QVariant &arg5, const QVariant &arg6, const QVariant &arg7,
		  const QVariant &arg8, const QVariant &arg9, const QVariant &arg10, const QVariant &arg11,
		  const QVariant &arg12, const QVariant &arg13, const QVariant &arg14, const QVariant &arg15,
		  const QVariant &arg16, const QVariant &arg17, const QVariant &arg18, const QVariant &arg19) : args(20)
	{
		this->id = id;
		QVarLengthArray<QVariant, 10> &args = *const_cast<QVarLengthArray<QVariant, 10>*>(&this->args);
		args[0] = arg0; args[1] = arg1; args[2] = arg2; args[3] = arg3; args[4] = arg4;
		args[5] = arg5; args[6] = arg6; args[7] = arg7; args[8] = arg8; args[9] = arg9;
		args[10] = arg10; args[11] = arg11; args[12] = arg12; args[13] = arg13; args[14] = arg14;
		args[15] = arg15; args[16] = arg16; args[17] = arg17; args[18] = arg18; args[19] = arg19;
	}

	void Event::send()
	{
		EventManager::send(*this);
	}

	template<typename TMethod>
	Q_INLINE_TEMPLATE quint16 EventManager::registerEventHandler(const char *id, QObject *obj,
																 TMethod method, quint16 priority)
	{
		return register_helper(obj, static_cast<Method>(method), id, priority);
	}

	LIBQUTIM_EXPORT QDebug operator<<(QDebug, const Event &);
}

#endif // EVENT_H
