/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Ruslan Nigmatullin euroelessar@yandex.ru
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
#ifndef AUTHORIZATIONDIALOG_H
#define AUTHORIZATIONDIALOG_H

#include "libqutim_global.h"
#include <QEvent>


namespace qutim_sdk_0_3
{

class Contact;
namespace Authorization
{

/**
* Send AuthorizationRequest to @param contact
*/
//FIXME create new notification filter-like API
class LIBQUTIM_EXPORT Request  : public QEvent
{
public:
	Request(Contact *contact, const QString &body = QString())
		:	QEvent(eventType()), m_contact(contact), m_body(body) {}
	Contact *contact() const { return m_contact; }
	QString body() const { return m_body; }
	/** @reimp */
	static QEvent::Type eventType();
private:
	Type m_type;
	Contact *m_contact;
	QString m_body;
};

class LIBQUTIM_EXPORT Reply  : public QEvent
{
public:
	enum Type {
		New,
		Accept,
		Reject,
		Accepted,
		Rejected
	};
	Reply(Type type, Contact *contact,const QString &body = QString())
		:	QEvent(eventType()), m_type(type), m_contact(contact), m_body(body) {}
	Contact *contact() const { return m_contact; }
	QString body() const { return m_body; }
	Type replyType() const { return m_type; }
	void setType(Type type) { m_type = type; }
	/** @reimp */
	static QEvent::Type eventType();
private:
	Type m_type;
	Contact *m_contact;
	QString m_body;
};

LIBQUTIM_EXPORT QObject *service(); //alias

}//namespace Authorization

}//namespace qutim_sdk_0_3


#endif // AUTHORIZATIONDIALOG_H

