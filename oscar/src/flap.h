/****************************************************************************
 *  flap.h
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

#ifndef FLAP_H
#define FLAP_H

#include "dataunit.h"

class QIODevice;

namespace Icq
{

class FLAP : public DataUnit
{
public:
	FLAP(quint8 channel = 0x02);
	inline quint8 channel() const { return m_channel; }
	inline void setChannel(quint8 channel) { m_channel = channel; }
	inline quint16 seqNum() const { return m_sequence_number; }
	inline void setSeqNum(quint8 seqnum) { m_sequence_number = seqnum; }
	QByteArray toByteArray() const;
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

} // namespace Icq

#endif // FLAP_H
