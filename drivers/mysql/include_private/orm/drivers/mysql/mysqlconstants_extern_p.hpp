#pragma once
#ifndef ORM_DRIVERS_MYSQL_MYSQLCONSTANTS_EXTERN_P_HPP
#define ORM_DRIVERS_MYSQL_MYSQLCONSTANTS_EXTERN_P_HPP

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

    // Common chars
    extern const QChar DOT;
    extern const QChar SEMICOLON;
    extern const QChar COLON;
    extern const QChar DASH;
    extern const QChar EQ_C;
    extern const QChar BACKTICK;

    // Common strings
    extern const QString EMPTY;
    extern const QString COMMA;

    // Database related
    extern const QString QMYSQL;
//    extern const QString QPSQL;
//    extern const QString QSQLITE;

} // namespace Constants
} // namespace Orm::Drivers::MySql

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_DRIVERS_MYSQL_MYSQLCONSTANTS_EXTERN_P_HPP
