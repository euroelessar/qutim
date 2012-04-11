/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Rusanov Peter <peter.rusanov@gmail.com>
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

#include <QDataStream>
#include <QBuffer>
#include <QTcpSocket>
#include <qendian.h>

#include "mrimdebug.h"
#include "protoutils.h"

#include "mrimpacket.h"

MrimPacket::MrimPacket(PacketMode mode) : m_mode(mode)
{
	clear();
}

MrimPacket::~MrimPacket()
{
}

void MrimPacket::initHeader()
{
	static quint32 seq = static_cast<quint32>(qrand());
	qMemSet(&m_header, 0, sizeof(m_header));
	m_header.magic = CS_MAGIC;
	m_header.proto = PROTO_VERSION;
	m_header.seq = seq++;
}

void MrimPacket::setHeader(const QByteArray& header)
{
	debug(DebugVeryVerbose)<<"Input array size = " << header.count();
	clear();
	if (header.size() != HEADER_SIZE) {
		m_header.magic = 0xBADBEEF;
		return;
	}
	QDataStream in(header);
	in.setByteOrder(QDataStream::LittleEndian);
	in >> m_header.magic;
	in >> m_header.proto;
	in >> m_header.seq;
	in >> m_header.msg;
	in >> m_header.dlen;
	in >> m_header.from;
	in >> m_header.fromport;
}

void MrimPacket::setHeader(const mrim_packet_header_t& header)
{
	m_header = header;
}

void MrimPacket::setBody(const QByteArray& body)
{    
	m_body.clear();
	m_body.append(body);
	m_header.dlen = m_body.length();
}

void MrimPacket::setBody(const QString& body)
{
	setBody(body.toAscii());
}

void MrimPacket::setBody(const char *body)
{
	Q_ASSERT(body);
	setBody(QByteArray(body));
}

void MrimPacket::setMsgType(quint32 msgType)
{
	m_header.msg = msgType;
}

void MrimPacket::setFrom(quint32 from)
{
	m_header.from = from;
}

void MrimPacket::setFromPort(quint32 fromPort)
{
	m_header.fromport = fromPort;
}

void MrimPacket::setSequence(quint32 seq)
{
	m_header.seq = seq;
}

QByteArray MrimPacket::toByteArray()
{
	QByteArray data;
	{
		QDataStream stream(&data, QIODevice::WriteOnly);
		stream.setByteOrder(QDataStream::LittleEndian);
		stream << m_header.magic;
		stream << m_header.proto;
		stream << m_header.seq;
		stream << m_header.msg;
		stream << m_header.dlen;
		stream << m_header.from;
		stream << m_header.fromport;
	}
	int oldSize = data.size();
	data.resize(HEADER_SIZE);
	qMemSet(data.data() + oldSize, 0, data.size() - oldSize);
	data += m_body;
	return data;
}

bool MrimPacket::readFrom(QIODevice& device)
{
	Q_ASSERT(mode() == Recieve);
	
	if (state() == ReadHeader)
	{
		m_headerData += device.read(HEADER_SIZE - m_headerData.size());
		if (m_headerData.size() != HEADER_SIZE)
			return true;
		
		setHeader(m_headerData);
		m_headerData.clear();
		
		if (isHeaderCorrect())
		{
			debug(DebugVeryVerbose)<<"Packet body size:" << dataLength();
			m_bytesLeft = dataLength();
			m_body.resize(dataLength());
			setState(ReadData);
		}
		else
		{
			setError(HeaderCorrupted);
		}
	}
	
	if (state() == ReadData && m_bytesLeft != 0)
	{
		char *data = m_body.data() + m_body.size() - m_bytesLeft;
		qint64 bytesRead = device.read(data,m_bytesLeft);
		
		if (bytesRead < 0)
		{//read error, connection lost
			setError(CannotReadFromSocket);
			return false;
		}
		m_bytesLeft -= bytesRead;
	}
	
	if (m_bytesLeft == 0)
	{
		setState(Finished);
	}
	return true;
}

QString MrimPacket::errorString(PacketError errCode)
{
	switch (errCode)
	{
	case NoError:
		return tr("No error");
	case CannotReadFromSocket:
		return tr("Cannot read from socket");
	case HeaderCorrupted:
		return tr("Header is corrupted");
	default:
		return tr("Unknown error");
	}
	return QString();
}

bool MrimPacket::isHeaderCorrect()
{
	return (m_header.magic == CS_MAGIC && dataLength() <= MAX_PACKET_BODY_SIZE);
}

void MrimPacket::append( const QString &str, bool unicode )
{
	LPString lpStr(str,unicode);
	append(lpStr);
}

void MrimPacket::append( LPString &lpStr )
{
	m_body.append(lpStr.toByteArray());
	m_header.dlen = m_body.length();
}

void MrimPacket::append( const quint32 &num )
{
	m_body.append(ByteUtils::toByteArray(num));
	m_header.dlen = m_body.length();
}

qint64 MrimPacket::writeTo(QIODevice *device, bool waitForWritten)
{
	Q_ASSERT(mode() == Compose);
	Q_ASSERT(device);
	qint64 written = device->write(toByteArray());
	
	if (waitForWritten)
	{
		device->waitForBytesWritten(10000);
	}
	return written;
}

MrimPacket& MrimPacket::operator<<(const QString &str)
{
	append(str);
	return *this;
}

MrimPacket& MrimPacket::operator<<( LPString &str )
{
	append(str);
	return *this;
}

MrimPacket& MrimPacket::operator<<( const quint32 &num )
{
	append(num);
	return *this;
}

qint32 MrimPacket::readTo(LPString &str, bool unicode)
{
	m_currBodyPos += str.read(data(),m_currBodyPos,unicode);
	return 0; //TODO: not need to return anything yet
}

qint32 MrimPacket::readTo( QString *str, bool unicode )
{
	Q_ASSERT(str);
	*str = ByteUtils::readString(data(),m_currBodyPos,unicode);
	m_currBodyPos += sizeof(quint32);
	m_currBodyPos += str->size() * ((unicode) ? 2 : 1);
	return str->size();
}

qint32 MrimPacket::readTo( quint32 &num )
{
	num = ByteUtils::readUint32(data(),m_currBodyPos);
	m_currBodyPos += sizeof(num);
	return sizeof(num);
}

void MrimPacket::setState(PacketState newState)
{
	m_currState = newState;
}

void MrimPacket::clear()
{
	initHeader();
	m_body.clear();
	m_currBodyPos = 0;
	m_bytesLeft = -1;
	setState(ReadHeader);
}

void MrimPacket::setError(PacketError errCode)
{
	m_lastError = errCode;
	setState(Error);
}

QDebug operator<<(QDebug dbg, const MrimPacket &p)
{
	dbg.nospace() << "MrimPacket (type=0x" << hex << p.msgType() << dec
			<< ", seq=" << p.sequence()
			<< ", dlen=" << p.dataLength()
			<< ", from=" << p.from()
			<< ", fromPort=" << p.fromPort()
			<< ", data="
			<< p.data().toHex().toUpper()
			<< ")";
	return dbg.space();
}

