/*****************************************************************************
    MRIMUtils

    Copyright (c) 2009 by Rusanov Peter <tazkrut@mail.ru>

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*****************************************************************************/

#include <QString>
#include <QDataStream>
#include <QBuffer>
#include <qendian.h>
#include <QApplication>
#include <QDesktopWidget>

#include "protoutils.h"

LPString::LPString(QString str, bool unicode)
    : m_rawData(NULL), m_unicode(unicode)
{
    m_string = new QString(str);
}

LPString::LPString(const QByteArray& arr, bool unicode)
    : m_rawData(NULL), m_string(NULL), m_unicode(unicode)
{
    read(arr);
}

LPString::LPString(const char* str, bool unicode) : m_rawData(NULL), m_string(NULL), m_unicode(unicode)
{
    read(QByteArray(str));
}

void LPString::read(const QByteArray& arr)
{
    QString codepage = (m_unicode) ? "UTF-16LE" : "CP1251";
    QTextCodec* codec = QTextCodec::codecForName(codepage.toLocal8Bit());

    if (codec != NULL)
    {
        delete m_string;
        QTextCodec::ConverterState convState(QTextCodec::IgnoreHeader);
        m_string = new QString(codec->toUnicode(arr.constData(),arr.length(),&convState));
    }
}

const QByteArray& LPString::toByteArray()
{
    if (!m_rawData)
    {
        m_rawData = new QByteArray;
    }

    QString codepage = (m_unicode) ? "UTF-16LE" : "CP1251";
    QTextCodec* codec = QTextCodec::codecForName(codepage.toLocal8Bit());
    
    if (codec != NULL)
    {
        QByteArray d;
        QTextCodec::ConverterState convState(QTextCodec::IgnoreHeader);

        if (m_string->size() > 0)
        {
        d.append(codec->fromUnicode(m_string->data(),m_string->size(),&convState));
        }
        m_rawData->append(ByteUtils::toByteArray(d.length()));
        m_rawData->append(d);
    }
    return *m_rawData;
}

const QString& LPString::toString()
{
    return *m_string;
}

LPString::~LPString()
{
    delete m_rawData;
    delete m_string;
}


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

quint32 ByteUtils::readUint32(QBuffer& buffer)
{
    return toUint32(buffer.read(4));
}

quint32 ByteUtils::readUint32(const QByteArray& arr, quint32 pos)
{
    return toUint32(arr.mid(pos,4));
}

LPString* ByteUtils::readLPS(QBuffer& buffer, bool unicode)
{
    quint32 len = readUint32(buffer);
    QByteArray str;
    str.append(buffer.read(len));
    LPString* lps = new LPString(str,unicode);
    return lps;
}

LPString* ByteUtils::readLPS(const QByteArray& arr, quint32 pos, bool unicode)
{
    quint32 len = readUint32(arr,pos);
    QByteArray str;
    str.append(arr.mid(pos+sizeof(len),len));
    LPString* lps = new LPString(str,unicode);
    return lps;
}


QString ByteUtils::readString(QBuffer& buffer, bool unicode)
{	
    LPString* lps = readLPS(buffer,unicode);
    QString res(lps->toString());
    delete lps;
    return res;
}

QString ByteUtils::readString( const QByteArray& arr, quint32 pos, bool unicode /*= false*/ )
{
    LPString* lps = readLPS(arr,pos,unicode);
    QString res(lps->toString());
    delete lps;
    return res;
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
