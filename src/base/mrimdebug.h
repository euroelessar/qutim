/****************************************************************************
 *  mrimdebug.h
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

#ifndef MRIMDEBUG_H
#define MRIMDEBUG_H

#include <QDebug>

enum TMrimDebugLevel
{
    VeryVerbose = 0,
    Verbose = 1,
    Info = 2,
    Disabled = 3
};

#ifdef QT_DEBUG
    #ifndef MRIM_DEBUG_LEVEL
        #define MRIM_DEBUG_LEVEL VeryVerbose
    #endif

    #define MDEBUG_EX(DebugLevel,DebugStatement,DebugObject) if (MRIM_DEBUG_LEVEL <= DebugLevel) { DebugObject<<"[MRIM]"<<":"<<DebugStatement; }

    #define MDEBUG(DebugLevel,DebugStatement) MDEBUG_EX(DebugLevel,DebugStatement,qDebug())
    #define MWARN(DebugStatement) MDEBUG_EX(Info,DebugStatement,qWarning())
    #define MCRIT(DebugStatement) MDEBUG_EX(Info,DebugStatement,qCritical())

#else
    #define MDEBUG(DebugLevel,DebugStatement)
#endif

#endif // MRIMDEBUG_H
