#include <qglobal.h>

#if defined(ADIUMWEBVIEW_LIBRARY)
#  define ADIUMWEBVIEW_EXPORT Q_DECL_EXPORT
#else
#  define ADIUMWEBVIEW_EXPORT Q_DECL_IMPORT
#endif
