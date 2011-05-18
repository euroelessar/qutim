/****************************************************************************
 *  utils.h
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

#ifndef UTILS_H
#define UTILS_H

#include <QString>
#include <QHostAddress>

class Utils
{
public:
    static QString stripEmail(const QString& email);
    static QString toHostPortPair(const QHostAddress& host, quint32 port);
};

#include "mrimdebug.h"

#endif // UTILS_H
