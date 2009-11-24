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
        #define MRIM_DEBUG_LEVEL Info
    #endif

    static TMrimDebugLevel MrimDebugLevel = MRIM_DEBUG_LEVEL;

    #define MDEBUG(DebugLevel,DebugStatement) if (MrimDebugLevel <= DebugLevel) { qDebug()<<"[MRIM]"<<Q_FUNC_INFO<<":"<<DebugStatement; }
#else
    #define MDEBUG(DebugLevel,DebugStatement)
#endif

#endif // MRIMDEBUG_H
