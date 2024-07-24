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
#include <QVariantHash>
#include <QVersionNumber>

#ifndef QT_NO_DEBUG_STREAM
#  include <QDebug>
#endif

#ifdef TINYORM_USING_QTSQLDRIVERS
#  include <QtSql/QSqlError>
#  include <QtSql/QSqlQuery>
#  include <QtSql/QSqlRecord>
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
#endif
