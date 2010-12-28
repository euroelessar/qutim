#ifndef GLOBAL_H
#define GLOBAL_H

#include <QtGlobal>

#ifdef W7QTWRAPPER
#	define W7QTEXPORT Q_DECL_EXPORT
#	define SegFault ((int(*)())0)
#else
#	define W7QTEXPORT Q_DECL_IMPORT
#endif

#endif
