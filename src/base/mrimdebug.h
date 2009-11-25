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

    #define MDEBUG_EX(DebugLevel,DebugStatement,DebugObject) if (MrimDebugLevel <= DebugLevel) { DebugObject<<"[MRIM]"<<Q_FUNC_INFO<<":"<<DebugStatement; }

    #define MDEBUG(DebugLevel,DebugStatement) MDEBUG_EX(DebugLevel,DebugStatement,qDebug())
    #define MWARN(DebugStatement) MDEBUG_EX(Info,DebugStatement,qWarning())
    #define MCRIT(DebugStatement) MDEBUG_EX(Info,DebugStatement,qCritical())

#else
    #define MDEBUG(DebugLevel,DebugStatement)
#endif

#endif // MRIMDEBUG_H
