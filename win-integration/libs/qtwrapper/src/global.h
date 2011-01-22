#ifndef GLOBAL_H
#define GLOBAL_H

#include <QtGlobal>

#ifdef WT_QT_WRAPPER
#	define WTQTEXPORT Q_DECL_EXPORT
#	define SegFault ((int(*)())0)
#else
#	define WTQTEXPORT Q_DECL_IMPORT
#endif

#endif // GLOBAL_H
