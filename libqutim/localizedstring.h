#ifndef LOCALIZEDSTRING_H
#define LOCALIZEDSTRING_H

#include "libqutim_global.h"

namespace qutim_sdk_0_3
{
	class LocalizedString
	{
	public:
		LocalizedString(const char *cxt, const QByteArray &str) : m_ctx(cxt), m_str(str) {}
		LocalizedString(const char *cxt, const char *str, int len) : m_ctx(cxt), m_str(str, len) {}
		LocalizedString(const char *cxt, const char *str) : m_ctx(cxt), m_str(str, qstrlen(str)) {}
		LocalizedString(const LocalizedString &other) : m_ctx(other.m_ctx), m_str(other.m_str) {}
		LocalizedString &operator =(const char *str) { m_str = str; return *this; }
		operator QString() const { return toString(); }
		QString toString() const;
		QByteArray original() const { return m_str; }
		QByteArray context() const { return m_ctx; }
	private:
		QByteArray m_ctx;
		QByteArray m_str;
	};
}

#endif // LOCALIZEDSTRING_H
