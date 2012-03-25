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

#ifndef STATUS_H
#define STATUS_H

#include "localizedstring.h"
#include <QSharedData>
#include <QVariant>
#include <QIcon>
#include <QEvent>

namespace qutim_sdk_0_3
{
class StatusPrivate;
class ExtendedStatusesEventPrivate;

class LIBQUTIM_EXPORT Status
{
public:
	enum Type
	{
		Connecting = -1,
		Online = 0,
		FreeChat,
		Away,
		NA,
		DND,
		Invisible,
		Offline
	};

	enum ChangeReason
	{
		ByUser,
		ByIdle,
		ByAuthorizationFailed,
		ByNetworkError,
		ByFatalError
	};

	// TODO: Remove defines, and optimize code, currently I just want plugins to compile
#if 0
	explicit
#endif
	Status(Type type = Offline);
	Status(const Status &other);
	Status &operator =(const Status &other);
	Status &operator =(Type type);
	virtual ~Status();

	bool operator ==(Type type) const;
	inline bool operator !=(Type type) const { return !operator ==(type); }

	QString text() const;
	void setText(const QString &text);
	LocalizedString name() const;
	void setName(const LocalizedString &name);
	QIcon icon() const;
	void setIcon(const QIcon &icon);
	Type type() const;
	void setType(Type type);
	int subtype() const;
	void setSubtype(int stype);
	template <typename T> T subtype() const { return static_cast<T>(subtype()); }
	template <typename T> void setSubtype(T stype) { setSubtype(static_cast<int>(stype)); }
	ChangeReason changeReason() const;
	void setChangeReason(ChangeReason reason);

	QVariant property(const char *name, const QVariant &def) const;
	template<typename T>
	T property(const char *name, const T &def) const
	{ return qVariantValue<T>(property(name, qVariantFromValue<T>(def))); }
	void setProperty(const char *name, const QVariant &value);

	void initIcon(const QString &protocol = QString());
	static QIcon createIcon(Type type, const QString &protocol = QString());
	static QString iconName(Type type, const QString &protocol = QString());
	static Status instance(Type type, const char *proto, int subtype = 0);
	static bool remember(const Status &status, const char *proto);
	static Status createConnecting(const Status &status, const char *proto);
	static Status connectingGoal(const Status &status);
	Status connectingGoal() const;
	void setExtendedInfo(const QString &name, const QVariantHash &status);
	void removeExtendedInfo(const QString &name);
	QVariantHash extendedInfo(const QString &name) const;
	QHash<QString, QVariantHash> extendedInfos() const;
	void setExtendedInfos(const QHash<QString, QVariantHash> &extInfos);
private:
	QSharedDataPointer<StatusPrivate> d;
};

/**
   The ExtendedInfosEvent class is used to request supported extended status infos
   from a protocol.
 */
class LIBQUTIM_EXPORT ExtendedInfosEvent : public QEvent
{
public:
	ExtendedInfosEvent();
	~ExtendedInfosEvent();
	void addInfo(const QString &name, const QVariantHash &info);
	QHash<QString, QVariantHash> infos();
	static QEvent::Type eventType();
private:
	QScopedPointer<ExtendedStatusesEventPrivate> d;
};

}

class QDebug;
LIBQUTIM_EXPORT QDebug operator<<(QDebug debug, qutim_sdk_0_3::Status::Type status);
LIBQUTIM_EXPORT QDebug operator<<(QDebug debug, const qutim_sdk_0_3::Status &status);
LIBQUTIM_EXPORT QDataStream &operator<<(QDataStream &out, const qutim_sdk_0_3::Status &status);
LIBQUTIM_EXPORT QDataStream &operator>>(QDataStream &in, qutim_sdk_0_3::Status &status);

Q_ENUMS(qutim_sdk_0_3::Status::Type)
Q_ENUMS(qutim_sdk_0_3::Status::ChangeReason)
Q_DECLARE_METATYPE(qutim_sdk_0_3::Status)
Q_DECLARE_METATYPE(qutim_sdk_0_3::Status::ChangeReason)

#endif // STATUS_H

