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

#ifndef MrimPacket_H_
#define MrimPacket_H_

#include <QDebug>
#include <QObject>
#include <QString>
#include <QByteArray>

#include "proto.h"

class QIODevice;
class LPString;
class QTcpSocket;

class PacketHandler
{
public:
	virtual ~PacketHandler() {}
    virtual QList<quint32> handledTypes() = 0;
    virtual bool handlePacket(class MrimPacket& packet) = 0;
};

class MrimPacket : public QObject
{
    Q_OBJECT
    Q_PROPERTY(quint32 msgType READ msgType WRITE setMsgType);
    Q_PROPERTY(quint32 from READ from WRITE setFrom);
    Q_PROPERTY(quint32 fromPort READ fromPort WRITE setFromPort);
    Q_PROPERTY(quint32 sequence READ sequence WRITE setSequence);

public:
    enum PacketError
    {
        NoError = 0,
        HeaderCorrupted,
        CannotReadFromSocket,
        UnknownError
    };

    enum PacketState
    {
        ReadHeader = 0,
        ReadData,
        Finished,
        Error
    };

    enum PacketMode
    {
        Receive,
        Compose
    };

public:	

    MrimPacket(PacketMode mode = Receive);
    virtual ~MrimPacket();

    static QString errorString(PacketError errCode);

    //Compose mode
    void append(const QString &str, bool unicode = false);
    void append(LPString &lpStr);
    void append(const quint32 &num);
    MrimPacket& operator<<(const QString &str);
    MrimPacket& operator<<(LPString &str);
    MrimPacket& operator<<(const quint32 &num);
    void setHeader(const QByteArray& header);
    void setHeader(const mrim_packet_header_t& header);
    void setBody(const QByteArray& body);
    void setBody(const QString& body);
    void setBody(const char *body);
    void setSequence(quint32 seq);
    void setMsgType(quint32 msgType);
    void setFrom(quint32 from);
    void setFromPort(quint32 fromPort);
    QByteArray toByteArray();
    qint64 writeTo(QIODevice *device, bool waitForWritten = false);

    //Receive mode
    bool readFrom(QIODevice& device);
    qint32 readTo(LPString &str, bool unicode = false);
    qint32 readTo(QString *str, bool unicode = false);
    qint32 readTo(quint32 &num);
    inline quint32 currBodyPos() const;
    inline bool atEnd() const;
    
    //Common
    bool isHeaderCorrect();
    inline quint32 dataLength() const { return m_header.dlen; }
    inline quint32 msgType() const { return m_header.msg; }
    inline quint32 sequence() const { return m_header.seq; }
    inline quint32 from() const { return m_header.from; }
    inline quint32 fromPort() const { return m_header.fromport; }
    inline const QByteArray& data() const { return m_body; }

    inline PacketError lastError() const;
    inline QString lastErrorString() const;
    void clear();

    inline PacketState state() const;
    inline bool isFinished() const;
    inline PacketMode mode() const;

private:
    void initHeader();
    void setState(PacketState newState);
    void setError(PacketError errCode);

    mrim_packet_header_t m_header;
    mrim_connection_params_t m_connParams;
    QByteArray m_body;
	QByteArray m_headerData;
    quint32 m_currBodyPos;
    quint32 m_bytesLeft;
    PacketState m_currState;
    PacketError m_lastError;
    PacketMode m_mode;
};

inline MrimPacket::PacketState MrimPacket::state() const
{ return m_currState; }

inline bool MrimPacket::isFinished() const
{ return state() == Finished; }

inline MrimPacket::PacketError MrimPacket::lastError() const
{ return (state() == Error) ? m_lastError : NoError; }

inline QString MrimPacket::lastErrorString() const
{ return errorString(lastError()); }

inline MrimPacket::PacketMode MrimPacket::mode() const
{ return m_mode; }

inline quint32 MrimPacket::currBodyPos() const
{ return m_currBodyPos; }

inline bool MrimPacket::atEnd() const
{ return currBodyPos() >= dataLength(); }

QDebug operator<<(QDebug dbg, const MrimPacket &p);

#endif /*MrimPacket_H_*/

