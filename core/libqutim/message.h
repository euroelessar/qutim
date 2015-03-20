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
#include <QDateTime>
#include <QEvent>

class QScriptEngine;

namespace qutim_sdk_0_3
{
class ChatUnit;
class MessagePrivate;

class LIBQUTIM_EXPORT MessageUnitData
{
    Q_GADGET
    Q_PROPERTY(QString id READ id)
    Q_PROPERTY(QString title READ title)
    Q_PROPERTY(QString avatar READ avatar)
public:
    MessageUnitData();
    MessageUnitData(const QString &id, const QString &title, const QString &avatar);

    QString id() const;
    QString title() const;
    QString avatar() const;

private:
    QString m_id;
    QString m_title;
    QString m_avatar;
};

class LIBQUTIM_EXPORT Message
{
    Q_GADGET
    Q_PROPERTY(QString text READ text WRITE setText)
    Q_PROPERTY(QString html READ html WRITE setHtml)
    Q_PROPERTY(QDateTime time READ time WRITE setTime)
    Q_PROPERTY(bool incoming READ isIncoming WRITE setIncoming)
    Q_PROPERTY(bool action READ isAction)
    Q_PROPERTY(qutim_sdk_0_3::ChatUnit* chatUnit READ chatUnit WRITE setChatUnit)
    Q_PROPERTY(quint64 id READ id)
    Q_PROPERTY(qutim_sdk_0_3::MessageUnitData unitData READ unitData)
public:
    struct UnitData
    {
        QString id;
		QString title;
		QString avatar;
    };

    enum SimiliarFlag
    {
        NoFlag = 0x00,
        IgnoreActions = 0x01,
    };
    Q_ENUM(SimiliarFlag)

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
    Q_INVOKABLE QString formatTime(const QString &format);
	void setIncoming(bool input);
	bool isIncoming() const;
    bool isAction() const;
	void setChatUnit (ChatUnit *chatUnit);
	ChatUnit *chatUnit() const;
    MessageUnitData unitData() const;
    QString unitName() const;
    QString unitId() const;
    QString unitAvatar() const;
	quint64 id() const;
	QVariant property(const char *name, const QVariant &def = QVariant()) const;
	template<typename T>
    T property(const char *name, const T &def) const;
	void setProperty(const char *name, const QVariant &value);
	QList<QByteArray> dynamicPropertyNames() const;

    Q_INVOKABLE QVariant property(const QString &name, const QVariant &def) const;
    Q_INVOKABLE bool hasProperty(const QString &name) const;
    Q_INVOKABLE QString formattedHtml() const;
    // FIXME: replace int flags by QFlags<SimiliarFlag> once QML would understand that
    Q_INVOKABLE bool isSimiliar(const qutim_sdk_0_3::Message &other, int flags = 0) const;
private:
	QSharedDataPointer<MessagePrivate> p;
};

template<typename T>
T Message::property(const char *name, const T &def) const
{
    QVariant var = property(name, QVariant::fromValue<T>(def));
    return var.value<T>();
}


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

LIBQUTIM_EXPORT QDebug operator<<(QDebug dbg, const qutim_sdk_0_3::Message &msg);

Q_DECLARE_METATYPE(qutim_sdk_0_3::MessageUnitData)
Q_DECLARE_METATYPE(qutim_sdk_0_3::Message)
Q_DECLARE_METATYPE(qutim_sdk_0_3::Message*)
Q_DECLARE_METATYPE(qutim_sdk_0_3::MessageList)

#endif // LIBQUTIM_MESSAGE_H

