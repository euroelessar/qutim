/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#ifndef FLAP_H
#define FLAP_H

#include "dataunit.h"

class QIODevice;

namespace qutim_sdk_0_3 {

namespace oscar {

class LIBOSCAR_EXPORT FLAP : public DataUnit
{
public:
	FLAP(quint8 channel = 0x02);
	inline quint8 channel() const { return m_channel; }
	inline void setChannel(quint8 channel) { m_channel = channel; }
	inline quint16 seqNum() const { return m_sequence_number; }
	inline void setSeqNum(quint16 seqnum) { m_sequence_number = seqnum; }
	QByteArray toByteArray() const;
	operator QByteArray() const { return toByteArray(); };
	QByteArray header() const;
	bool readData(QIODevice *dev);
	inline bool isFinished() const { return m_state == Finished; }
	void clear();
private:
	enum State { ReadHeader, ReadData, Finished } m_state;
	quint8 m_channel;
	quint16 m_sequence_number;
	quint16 m_length;
};

} } // namespace qutim_sdk_0_3::oscar

#endif // FLAP_H

