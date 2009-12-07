#include <qglobal.h>
 // XDG_LIBRARY
 // defined(1)
 // Q_DECL_EXPORT
#if defined(XDG_LIBRARY)
# define XDG_API Q_DECL_EXPORT
#else
# define XDG_API Q_DECL_IMPORT
#endif