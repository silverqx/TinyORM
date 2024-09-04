#pragma once
#ifndef ORM_DRIVERS_MYSQL_MYSQLCONSTANTS_INLINE_P_HPP
#define ORM_DRIVERS_MYSQL_MYSQLCONSTANTS_INLINE_P_HPP

#include <QString>

#include <orm/macros/commonnamespace.hpp>

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Drivers::MySql
{
    /*! Alias for the literal operator that creates a QLatin1Char/StringView. */
    using Qt::StringLiterals::operator""_L1;
    /*! Alias for the literal operator that creates a QString. */
    using Qt::StringLiterals::operator""_s;

/*! Namespace contains common chars and strings used in the TinyMySql project. */
namespace Constants
{

    /*! Alias for the literal operator that creates a QString. */
    using Qt::StringLiterals::operator""_s;

    // Common chars - QChar(u'') is faster than ''_L1
    inline const QChar DOT       = QChar(u'.');
    inline const QChar SEMICOLON = QChar(u';');
    inline const QChar COLON     = QChar(u':');
    inline const QChar DASH      = QChar(u'-');
    inline const QChar EQ_C      = QChar(u'=');
    inline const QChar BACKTICK  = QChar(u'`');

    // Common strings
    inline const QString EMPTY   = u""_s; // This is the fastest
    inline const QString COMMA   = u", "_s;

    // Database related
    inline const QString QMYSQL  = u"QMYSQL"_s;
//  inline const QString QPSQL   = u"QPSQL"_s;
//  inline const QString QSQLITE = u"QSQLITE"_s;

} // namespace Constants
} // namespace Orm::Drivers::MySql

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_DRIVERS_MYSQL_MYSQLCONSTANTS_INLINE_P_HPP
