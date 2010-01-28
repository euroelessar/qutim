/****************************************************************************
 *  snac.cpp
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

#include "snac.h"
#include "util.h"

namespace Icq
{

void SNAC::reset(quint16 family, quint16 subtype)
{
	m_family = family;
	m_subtype = subtype;
	m_flags = 0;
	m_data.clear();
}

SNAC SNAC::fromByteArray(const QByteArray &data)
{
	quint16 family = qFromBigEndian<quint16> ((const uchar *) data.constData());
	quint16 subtype = qFromBigEndian<quint16> ((const uchar *) data.constData() + 2);
	SNAC snac(family, subtype);
	snac.m_flags = qFromBigEndian<quint16> ((const uchar *) data.constData() + 4);
	snac.m_id = qFromBigEndian<quint32> ((const uchar *) data.constData() + 6);
	snac.m_data = data.mid(10);
	return snac;
}

QByteArray SNAC::toByteArray() const
{
	QByteArray data = header();
	data += m_data;
	return data;
}

QByteArray SNAC::header() const
{
	QByteArray data;
	data.resize(10);
	qToBigEndian<quint16> (m_family, (uchar *) data.data());
	qToBigEndian<quint16> (m_subtype, (uchar *) data.data() + 2);
	qToBigEndian<quint16> (m_flags, (uchar *) data.data() + 4);
	qToBigEndian<quint32> (m_id, (uchar *) data.data() + 6);
	return data;
}

} // namespace Icq
