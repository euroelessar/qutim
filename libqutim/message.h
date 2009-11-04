/****************************************************************************
 *  message.h
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

#ifndef LIBQUTIM_MESSAGE_H
#define LIBQUTIM_MESSAGE_H

#include "libqutim_global.h"
#include <QSharedData>
#include <QVariant>

namespace qutim_sdk_0_3
{
	class Contact;
	class MessagePrivate;

	class LIBQUTIM_EXPORT Message
	{
	public:
		Message();
		Message(const QString &text);
		Message(const Message &other);
		virtual ~Message();
		const QString &text() const;
		void setText(const QString &text);
		const QDateTime &time() const;
		void setTime(const QDateTime &time);
		void setIncoming(bool input);
		bool isIncoming() const;
		void setContact (Contact *contact);
		const Contact *contact() const;
		QVariant property(const char *name) const;
		void setProperty(const char *name, const QVariant &value);
		QList<QByteArray> dynamicPropertyNames() const;
	private:
		QSharedDataPointer<MessagePrivate> p;
	};
}

#endif // LIBQUTIM_MESSAGE_H
