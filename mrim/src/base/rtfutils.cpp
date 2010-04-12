/****************************************************************************
 *  rtfutils.cpp
 *
 *  Copyright (c) 2010 by Rusanov Peter <peter.rusanov@gmail.com>
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

#include <QtEndian>
#include <QBuffer>

#ifndef NO_RTF_SUPPORT
#include <rtf/rtftextreader.h>

#include "protoutils.h"
#include "rtfutils.h"

class RtfPrivate
{
public:
    RtfTextReader *reader;
};

Rtf::Rtf(const char *defaultEncoding) :
    p(new RtfPrivate)
{
    p->reader = new RtfTextReader(defaultEncoding);
}

Rtf::~Rtf() {
    delete p->reader;
}

QString Rtf::toPlainText(RtfTextReader *reader, const QString& rtfMsg)
{
	QByteArray unbased = QByteArray::fromBase64(rtfMsg.toLatin1());
    QByteArray arr;
    quint32 beLen = qToBigEndian(unbased.length()*10);
    arr.append(ByteUtils::toByteArray(beLen));
    arr.append(unbased);
    QByteArray uncompressed = qUncompress(arr);

    QBuffer buf;
    buf.open(QIODevice::ReadWrite);
    buf.write(uncompressed);
    buf.seek(0);
    quint32 numLps = ByteUtils::readUint32(buf);

    QString plainText;

    if (numLps > 1)
    {
        QByteArray rtfMsg = ByteUtils::readArray(buf);
        QString color = ByteUtils::readString(buf);//not used now
        Q_UNUSED(color);
        reader->readDocument(rtfMsg);
        plainText = reader->getText();
    }
    return plainText;
}

QString Rtf::toPlainText(const QString& rtfMsg)
{
    return toPlainText(p->reader,rtfMsg);
}

#endif //NO_RTF_SUPPORT
