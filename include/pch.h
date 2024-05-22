/* This file can't be included in the project, it's for a precompiled header. */

/* Add C includes here */

#ifdef __cplusplus
/* Add C++ includes here */
#include <QDateTime>
#include <QDir>
#include <QElapsedTimer>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QRegularExpression>
#include <QSet>
#include <QTimeZone>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlRecord>
#include <QVariantHash>
#include <QVersionNumber>

#ifndef QT_NO_DEBUG_STREAM
#  include <QDebug>
#endif
#ifdef _WIN32
#  include <qt_windows.h>
#endif

#include <fstream>
#include <set>
#include <stack>
#include <typeindex>
#include <typeinfo>
#include <unordered_set>

#ifdef __GNUG__
#  include <cxxabi.h>
#endif
#if defined(Q_OS_LINUX) && !defined(QT_LINUXBASE)
#  include <sys/prctl.h>
#endif

#include <range/v3/action/erase.hpp>
#include <range/v3/action/remove_if.hpp>
#include <range/v3/action/sort.hpp>
#include <range/v3/action/transform.hpp>
#include <range/v3/action/unique.hpp>
#include <range/v3/algorithm/contains.hpp>
#include <range/v3/algorithm/set_algorithm.hpp>
#include <range/v3/algorithm/stable_sort.hpp>
#include <range/v3/algorithm/unique.hpp>
#include <range/v3/iterator/insert_iterators.hpp>
#include <range/v3/numeric/accumulate.hpp>
#include <range/v3/range/conversion.hpp>
#include <range/v3/view/filter.hpp>
#include <range/v3/view/map.hpp>
#include <range/v3/view/move.hpp>
#include <range/v3/view/remove_if.hpp>
#include <range/v3/view/reverse.hpp>
#include <range/v3/view/set_algorithm.hpp>
#include <range/v3/view/transform.hpp>
#endif
