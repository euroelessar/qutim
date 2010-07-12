/****************************************************************************
 *  status.h
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

#ifndef STATUS_H
#define STATUS_H

#include "localizedstring.h"
#include <QSharedData>
#include <QVariant>
#include <QIcon>

namespace qutim_sdk_0_3
{
	class StatusPrivate;

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
		void setExtendedInfo(const QString &name, const QVariantMap &status);
		void removeExtendedInfo(const QString &name);
		QVariantMap extendedInfo(const QString &name) const;
		QVariantHash extendedInfos() const;
	private:
		QSharedDataPointer<StatusPrivate> d;
	};
}

class QDebug;
LIBQUTIM_EXPORT QDebug operator<<(QDebug debug, qutim_sdk_0_3::Status::Type status);
LIBQUTIM_EXPORT QDebug operator<<(QDebug debug, const qutim_sdk_0_3::Status &status);
Q_ENUMS(qutim_sdk_0_3::Status::Type)
Q_DECLARE_METATYPE(qutim_sdk_0_3::Status)

#endif // STATUS_H
