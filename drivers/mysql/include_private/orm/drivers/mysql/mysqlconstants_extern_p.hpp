#pragma once
#ifndef ORM_DRIVERS_MYSQL_MYSQLCONSTANTS_EXTERN_P_HPP
#define ORM_DRIVERS_MYSQL_MYSQLCONSTANTS_EXTERN_P_HPP

#include <QString>

#include <orm/macros/commonnamespace.hpp>

#include "orm/drivers/macros/export.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

/*! Namespace contains common chars and strings used in the TinyMySql project. */
namespace Orm::Drivers::MySql::Constants
{

    // Common chars
    TINYDRIVERS_EXPORT extern const QChar SEMICOLON;
    TINYDRIVERS_EXPORT extern const QChar COLON;
    TINYDRIVERS_EXPORT extern const QChar DASH;
    TINYDRIVERS_EXPORT extern const QChar EQ_C;

    // Common strings
    TINYDRIVERS_EXPORT extern const QString COMMA;

} // namespace Orm::Drivers::MySql::Constants

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_DRIVERS_MYSQL_MYSQLCONSTANTS_EXTERN_P_HPP
