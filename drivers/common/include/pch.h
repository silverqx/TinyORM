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

#include <mutex>        // IWYU pragma: keep
#include <shared_mutex> // IWYU pragma: keep
#include <thread>       // IWYU pragma: keep

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
