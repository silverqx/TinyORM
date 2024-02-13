#pragma once
#ifndef ORM_DRIVERS_MYSQL_MYSQLCONSTANTS_INLINE_P_HPP
#define ORM_DRIVERS_MYSQL_MYSQLCONSTANTS_INLINE_P_HPP

#include <QString>

#include <orm/macros/commonnamespace.hpp>

TINYORM_BEGIN_COMMON_NAMESPACE

/*! Namespace contains common chars and strings used in the TinyMySql project. */
namespace Orm::Drivers::MySql::Constants
{

    // Common chars
    inline const QChar SEMICOLON = QLatin1Char(';');
    inline const QChar COLON     = QLatin1Char(':');
    inline const QChar DASH      = QLatin1Char('-');
    inline const QChar EQ_C      = QLatin1Char('=');

    // Common strings
    inline const QString COMMA   = QStringLiteral(", ");

} // namespace Orm::Drivers::MySql::Constants

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_DRIVERS_MYSQL_MYSQLCONSTANTS_INLINE_P_HPP
