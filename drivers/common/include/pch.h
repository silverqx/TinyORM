/* This file can't be included in the project, it's for a precompiled header. */

/* Add C includes here */

#ifdef __cplusplus
/* Add C++ includes here */
#include <QDateTime>
#include <QDir>
#include <QElapsedTimer>
#include <QLibrary>
#include <QRegularExpression>
#include <QUuid>
#include <QVariant>
#include <QVersionNumber>

#ifndef QT_NO_DEBUG_STREAM
#  include <QDebug>
#endif
#ifdef _WIN32
#  include <qt_windows.h>
#endif

#include <shared_mutex>

#ifdef __linux__
#  include <dlfcn.h>
#endif

#include <orm/macros/archdetect.hpp>
#include <orm/macros/commonnamespace.hpp>
#include <orm/macros/compilerdetect.hpp>
#include <orm/macros/export_common.hpp>
#include <orm/macros/likely.hpp>
#include <orm/macros/stringify.hpp>
#include <orm/macros/systemheader.hpp>
#include <orm/support/replacebindings.hpp>
#endif
