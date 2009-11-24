/*****************************************************************************
    MrimPacket

    Copyright (c) 2009 by Rusanov Peter <peter.rusanov@gmail.com>

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*****************************************************************************/

#ifndef MrimPacket_H_
#define MrimPacket_H_

#include <QObject>
#include <QString>
#include <QByteArray>
#include "proto.h"

class QBuffer;
class LPString;
class QTcpSocket;

class MrimPacket : public QObject
{
    Q_OBJECT
    Q_PROPERTY(quint32 msgType READ msgType WRITE setMsgType);
    Q_PROPERTY(quint32 from READ from WRITE setFrom);
    Q_PROPERTY(quint32 fromPort READ fromPort WRITE setFromPort);
    Q_PROPERTY(quint32 sequence READ sequence WRITE setSequence);

public:
    enum TPacketErrorCode
    {
        NoError = 0,
        NotEnoughBytes,
        HeaderCorrupted,
        UnknownError
    };

public:	

    MrimPacket();
    virtual ~MrimPacket();

    static MrimPacket *fromRawData(QBuffer& buffer, TPacketErrorCode &err);
    void setHeader(const QByteArray& header);
    void setHeader(const mrim_packet_header_t& header);
    void setBody(const QByteArray& body);
    void setBody(const QString& body);
    void setBody(const char *body);
    void setSequence(quint32 seq);
    void setMsgType(quint32 msgType);
    void setFrom(quint32 from);
    void setFromPort(quint32 fromPort);

    void append(const QString &str, bool unicode = false);
    void append(LPString &lpStr);
    void append(const quint32 &num);

    qint32 read(QString *str,bool unicode = false);
    qint32 read(quint32 &num);

    MrimPacket& operator<<(LPString &str);
    MrimPacket& operator<<(const quint32 &num);

    bool isHeaderCorrect();
    inline quint32 dataLength() { return m_header.dlen; }
    inline quint32 msgType() { return m_header.msg; }
    inline quint32 sequence() { return m_header.seq; }
    inline quint32 from() { return m_header.from; }
    inline quint32 fromPort() { return m_header.fromport; }
    inline const QByteArray& data() { return m_body; }

    QByteArray toByteArray();
    qint64 send(QTcpSocket *aSocket);

private:
    void initHeader();

    mrim_packet_header_t m_header;
    mrim_connection_params_t m_connParams;
    QByteArray m_body;
    quint32 m_currBodyPos;
};

#endif /*MrimPacket_H_*/
