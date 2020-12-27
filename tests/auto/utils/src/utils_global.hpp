#ifndef UTILS_GLOBAL_H
#define UTILS_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(UTILS_BUILDING_SHARED)
#  define UTILS_EXPORT Q_DECL_EXPORT
#else
#  define UTILS_EXPORT Q_DECL_IMPORT
#endif

#endif // UTILS_GLOBAL_H
