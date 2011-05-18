/****************************************************************************
 *  rtfutils.h
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

#ifndef RTFUTILS_H
#define RTFUTILS_H

#include <QScopedPointer>
#include <QString>

class RtfTextReader;
class RtfPrivate;

class Rtf
{
public:
    Rtf(const char *defaultEncoding = "utf-8");
    ~Rtf();

    static void parse(RtfTextReader *reader, const QString& rtfMsg, QString *plainText, QString *html);
    void parse(const QString& rtfMsg, QString *plainText, QString *html);

private:
    QScopedPointer<RtfPrivate> p;    
};

#endif // RTFUTILS_H
