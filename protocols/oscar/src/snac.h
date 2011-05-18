/****************************************************************************
 *  snac.h
 *
 *  Copyright (c) 2010 by Nigmatullin Ruslan <euroelessar@gmail.com>
 *                        Prokhin Alexey <alexey.prokhin@yandex.ru>
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

#ifndef SNAC_H
#define SNAC_H

#include "tlv.h"
#include "cookie.h"

namespace qutim_sdk_0_3 {

namespace oscar {

class LIBOSCAR_EXPORT SNAC : public DataUnit
{
public:
	SNAC(quint16 family = 0, quint16 subtype = 0);
	~SNAC();
	void reset(quint16 family, quint16 subtype);
	inline quint16 family() const { return m_family; }
	inline void setFamily(SnacFamily family) { m_family = family; }
	inline quint16 subtype() const { return m_subtype; }
	inline void setSubtype(quint16 subtype) { m_subtype = subtype; }
	inline quint16 flags() const { return m_flags; }
	inline void setFlags(quint16 flags) { m_flags = flags; }
	inline quint32 id() const { return m_id; }
	inline void setId(quint32 id) { m_id = id; }
	static SNAC fromByteArray(const QByteArray &data);
	QByteArray toByteArray() const;
	QByteArray header() const;
	inline operator QByteArray() const { return toByteArray(); }
	inline bool isEmpty() const { return m_family == 0 && m_subtype == 0; }
	void setCookie(const Cookie &cookie, QObject *receiver = 0, const char *member = 0, int msec = 30000);
	void lock();
private:
	quint16 m_family;
	quint16 m_subtype;
	quint16 m_flags;
	quint32 m_id;
	Cookie m_cookie;
	QObject *m_receiver;
	QLatin1String m_member;
	int m_msec;
};

} } // namespace qutim_sdk_0_3::oscar

#endif // SNAC_H
