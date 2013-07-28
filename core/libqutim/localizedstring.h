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

#ifndef LOCALIZEDSTRING_H
#define LOCALIZEDSTRING_H

#include "libqutim_global.h"
#include <QMetaType>

#ifndef NO_AUTOLOCALIZATION
# undef QT_TR_NOOP
# undef QT_TR_NOOP_UTF8
# undef QT_TRANSLATE_NOOP
# undef QT_TRANSLATE_NOOP_UTF8
# undef QT_TRANSLATE_NOOP3
# undef QT_TRANSLATE_NOOP3_UTF8
# define QT_TR_NOOP(x)                            qutim_sdk_0_3::LocalizedString(staticMetaObject.className(), x)
# define QT_TR_NOOP_UTF8(x)                       qutim_sdk_0_3::LocalizedString(staticMetaObject.className(), x)
# define QT_TRANSLATE_NOOP(scope, x)              qutim_sdk_0_3::LocalizedString(scope, x)
# define QT_TRANSLATE_NOOP_UTF8(scope, x)         qutim_sdk_0_3::LocalizedString(scope, x)
# define QT_TRANSLATE_NOOP3(scope,x,comment)      qutim_sdk_0_3::LocalizedString(scope, x)
# define QT_TRANSLATE_NOOP3_UTF8(scope,x,comment) qutim_sdk_0_3::LocalizedString(scope, x)
#endif // NO_AUTOLOCALIZATION

namespace qutim_sdk_0_3 {
class LocalizedString;
}

QDataStream &operator<<(QDataStream &out, const qutim_sdk_0_3::LocalizedString &str);
QDataStream &operator>>(QDataStream &in, qutim_sdk_0_3::LocalizedString &str);

namespace qutim_sdk_0_3
{
	class LIBQUTIM_EXPORT LocalizedString
	{
	public:
		LocalizedString() {}
		LocalizedString(const char *str) : m_str(str) {}
		LocalizedString(const QByteArray &str) : m_str(str) {}
		LocalizedString(const QString &str) : m_str(str.toUtf8()) {}
		LocalizedString(const char *cxt, const QByteArray &str) : m_ctx(cxt), m_str(str) {}
		LocalizedString(const char *cxt, const char *str, int len) : m_ctx(cxt), m_str(str, len) {}
		LocalizedString(const char *cxt, const char *str) : m_ctx(cxt), m_str(str, qstrlen(str)) {}
		LocalizedString(const LocalizedString &other) : m_ctx(other.m_ctx), m_str(other.m_str) {}
		LocalizedString &operator =(const char *str) { m_str = str; return *this; }
		operator QString() const { return toString(); }
		QString toString() const;
		void setOriginal(const QByteArray &str) { m_str = str; }
		QByteArray original() const { return m_str; }
		void setContext(const QByteArray &ctx) { m_ctx = ctx; }
		QByteArray context() const { return m_ctx; }
		bool isNull() const { return m_str.isNull(); }

		inline bool operator==(const LocalizedString &s) const { return m_str == s.m_str; }
		inline bool operator <(const LocalizedString &s) const { return m_str < s.m_str; }
		inline bool operator >(const LocalizedString &s) const { return s < *this; }
		inline bool operator!=(const LocalizedString &s) const { return !operator==(s); }
		inline bool operator<=(const LocalizedString &s) const { return !operator>(s); }
		inline bool operator>=(const LocalizedString &s) const { return !operator<(s); }
	private:
		QByteArray m_ctx;
		QByteArray m_str;
	};

	typedef QList<LocalizedString> LocalizedStringList;
}

Q_DECLARE_METATYPE(qutim_sdk_0_3::LocalizedStringList);
Q_DECLARE_METATYPE(qutim_sdk_0_3::LocalizedString);

#endif // LOCALIZEDSTRING_H

