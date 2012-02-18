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

#ifndef PROTOUTILS_H_
#define PROTOUTILS_H_

#include <QByteArray>
#include <QString>
#include <QIODevice>

#include "proto.h"
#include "mrimdefs.h"
#include "lpstring.h"

class ByteUtils
{
public:
    static QByteArray toByteArray(const quint32 ul);
    static quint32 toUint32(const QByteArray& arr);
    static quint32 readUint32(QIODevice& aBuff);
    static quint32 readUint32(const QByteArray& arr, quint32 pos = 0);
    static LPString* readLPS(QIODevice& buffer, bool unicode = false);
    static LPString* readLPS(const QByteArray& arr, quint32 pos = 0, bool unicode = false);
    static QString readString(QIODevice& buffer, bool unicode = false);
    static QString readString(const QByteArray& arr, quint32 pos = 0, bool unicode = false);
    static QByteArray readArray(QIODevice& buffer);
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

