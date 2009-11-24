/*****************************************************************************
    protoutils.h

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

#ifndef PROTOUTILS_H_
#define PROTOUTILS_H_

#include <QByteArray>
#include <QString>
#include <QBuffer>
#include <QTextCodec>
#include <QIcon>

#include "proto.h"
#include "mrimdefs.h"

class LPString
{
public:
    LPString(QString str, bool unicode = false);
    LPString(const QByteArray& arr, bool unicode = false);
    LPString(const char* str, bool unicode = false);
    virtual ~LPString();

    static LPString* fromBuffer(QBuffer& buffer);
    void read(const QByteArray& arr);

    const QByteArray& toByteArray();
    const QString& toString();

private:
    QString* m_string;
    QByteArray* m_rawData;
    bool m_unicode;
};

class ByteUtils
{
public:
    static QByteArray toByteArray(const quint32 ul);
    static quint32 toUint32(const QByteArray& arr);
    static quint32 readUint32(QBuffer& aBuff);
    static quint32 readUint32(const QByteArray& arr, quint32 pos = 0);
    static LPString* readLPS(QBuffer& buffer, bool unicode = false);
    static LPString* readLPS(const QByteArray& arr, quint32 pos = 0, bool unicode = false);
    static QString readString(QBuffer& buffer, bool unicode = false);
    static QString readString(const QByteArray& arr, quint32 pos = 0, bool unicode = false);
};

/*
class MRIMCommonUtils
{
public:
    static QString ConvertToPlainText(QString aRtfMsg);
    static QPoint  DesktopCenter(QSize aWidgetSize);
    static QString GetFileSize(quint64 aSize);
};
*/

#endif /*PROTOUTILS_H_*/
