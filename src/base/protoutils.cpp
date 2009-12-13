/****************************************************************************
 *  protoutils.cpp
 *
 *  Copyright (c) 2009 by Rusanov Peter <peter.rusanov@gmail.com>
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

#include <QString>
#include <QDataStream>
#include <QTextCodec>
#include <qendian.h>

#include "protoutils.h"

QByteArray ByteUtils::toByteArray(const quint32 UL)
{
    QByteArray packet;
    packet[3] = (UL / 0x1000000);
    packet[2] = (UL / 0x10000);
    packet[1] = (UL / 0x100);
    packet[0] = (UL % 0x100);
    return packet;
}

quint32 ByteUtils::toUint32(const QByteArray& arr)
{
    bool ok;
    quint32 res = arr.toHex().toULong(&ok,16);
    res = qToBigEndian(res);
    return res;
}

quint32 ByteUtils::readUint32(QIODevice& buffer)
{
    return toUint32(buffer.read(4));
}

quint32 ByteUtils::readUint32(const QByteArray& arr, quint32 pos)
{
    return toUint32(arr.mid(pos,4));
}

LPString* ByteUtils::readLPS(QIODevice& device, bool unicode)
{
    return LPString::readFrom(device,unicode);
}

LPString* ByteUtils::readLPS(const QByteArray& arr, quint32 pos, bool unicode)
{
    return LPString::readFrom(arr,pos,unicode);
}

QString ByteUtils::readString(QIODevice& device, bool unicode)
{	
    LPString lps;
    lps.read(device,unicode);
    return lps.toString();
}

QString ByteUtils::readString( const QByteArray& arr, quint32 pos, bool unicode /*= false*/ )
{
    LPString lps;
    lps.read(arr,pos,unicode);
    return lps.toString();
}

//MRIMCommonUtils
/*
QString MRIMCommonUtils::ConvertToPlainText(QString aRtfMsg)
{
    QByteArray unbased = QByteArray::fromBase64(aRtfMsg.toAscii());
    QByteArray arr;
    quint32 beLen = qToBigEndian(unbased.length()*10);
    arr.append(ByteUtils::ConvertULToArray(beLen));
    arr.append(unbased);
    QByteArray uncompressed = qUncompress(arr);
#ifdef DEBUG_LEVEL_DEV
    qDebug()<<"Unpacked length: "<<uncompressed.count();
#endif
    QBuffer buf;
    buf.open(QIODevice::ReadWrite);
    buf.write(uncompressed);
    buf.seek(0);
    quint32 numLps = ByteUtils::ReadToUL(buf);
#ifdef DEBUG_LEVEL_DEV
    qDebug()<<"Number of string-params in message: "<<numLps;
#endif
    QString plainText;

    if (numLps > 1)
    {
        QString rtfMsg = ByteUtils::ReadToString(buf,false);
#ifdef DEBUG_LEVEL_DEV
    qDebug()<<"Unbased and unzipped rtf message: "<<rtfMsg;
#endif
        QString color = ByteUtils::ReadToString(buf);//not used now
        Q_UNUSED(color);
        RTFImport rtfConverter;
        plainText = rtfConverter.convert(rtfMsg);
    }
    return plainText;
}

QPoint MRIMCommonUtils::DesktopCenter(QSize aWidgetSize)
{
    QDesktopWidget &desktop = *QApplication::desktop();
    return QPoint(desktop.width() / 2 - aWidgetSize.width() / 2, desktop.height() / 2 - aWidgetSize.height() / 2);
}

QString MRIMCommonUtils::GetFileSize(quint64 aSize)
{
    quint64 bytes = aSize % 1024;
    quint32 kBytes = aSize % (1024 * 1024) / 1024;
    quint32 mBytes = aSize % (1024 * 1024 * 1024) / (1024 * 1024);
    quint32 gBytes = aSize / (1024 * 1024 * 1024);

    QString fileSize;

    if ( bytes && !kBytes && !mBytes && !gBytes )
            fileSize.append(QString::number(bytes) + tr(" B"));
    else if (kBytes && !mBytes && !gBytes )
            fileSize.append(QString::number(kBytes) + "," + QString::number(bytes) + tr(" KB"));
    else if (mBytes && !gBytes )
            fileSize.append(QString::number(mBytes) + "," + QString::number(kBytes) + tr(" MB"));
    else if (gBytes )
            fileSize.append(QString::number(gBytes) + "," + QString::number(mBytes) + tr(" GB"));

    return fileSize;
}
*/
