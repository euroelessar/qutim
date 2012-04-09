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

#ifndef LIBQUTIM_MESSAGE_H
#define LIBQUTIM_MESSAGE_H

#include "libqutim_global.h"
#include <QSharedData>
#include <QVariant>
#include <QEvent>

class QScriptEngine;

namespace qutim_sdk_0_3
{
class ChatUnit;
class MessagePrivate;

class LIBQUTIM_EXPORT Message
{
public:
	Message();
	Message(const QString &text);
	Message(const Message &other);
	virtual ~Message();
	Message &operator =(const Message &other);
	static void scriptRegister(QScriptEngine *engine);
	const QString &text() const;
	void setText(const QString &text);
	QString html() const;
	void setHtml(const QString &html);
	const QDateTime &time() const;
	void setTime(const QDateTime &time);
	void setIncoming(bool input);
	bool isIncoming() const;
	void setChatUnit (ChatUnit *chatUnit);
	ChatUnit *chatUnit() const;
	QString unitName() const;
	QString unitId() const;
	QString unitAvatar() const;
	quint64 id() const;
	QVariant property(const char *name, const QVariant &def = QVariant()) const;
	template<typename T>
	T property(const char *name, const T &def) const
	{ return qVariantValue<T>(property(name, qVariantFromValue<T>(def))); }
	void setProperty(const char *name, const QVariant &value);
	QList<QByteArray> dynamicPropertyNames() const;
private:
	QSharedDataPointer<MessagePrivate> p;
};

class LIBQUTIM_EXPORT MessageReceiptEvent : public QEvent
{
public:
	MessageReceiptEvent(quint64 id, bool success);
	inline quint64 id() const { return i; }
	inline bool success() const { return s; }
	static QEvent::Type eventType();
protected:
	quint64 i;
	bool s;
};

LIBQUTIM_EXPORT QString unescape(const QString &html);

typedef QList<Message> MessageList;
}

Q_DECLARE_METATYPE(qutim_sdk_0_3::Message)
Q_DECLARE_METATYPE(qutim_sdk_0_3::Message*)
Q_DECLARE_METATYPE(qutim_sdk_0_3::MessageList)

#endif // LIBQUTIM_MESSAGE_H

