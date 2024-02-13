#pragma once
#ifndef ORM_DRIVERS_CONSTANTS_EXTERN_P_HPP
#define ORM_DRIVERS_CONSTANTS_EXTERN_P_HPP

#include <QString>

#include <orm/macros/commonnamespace.hpp>

#include "orm/drivers/macros/export.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

/*! Namespace contains common chars and strings used in the TinyDrivers project. */
namespace Orm::Drivers::Constants
{

    // Common chars
    TINYDRIVERS_EXPORT extern const QChar SPACE;
    TINYDRIVERS_EXPORT extern const QChar DOT;
    TINYDRIVERS_EXPORT extern const QChar QUOTE;

    // Common strings
    TINYDRIVERS_EXPORT extern const QString NEWLINE;
    TINYDRIVERS_EXPORT extern const QString COMMA;
    TINYDRIVERS_EXPORT extern const QString null_;

    // Database related
    TINYDRIVERS_EXPORT extern const QString QMYSQL;
//    TINYDRIVERS_EXPORT extern const QString QPSQL;
//    TINYDRIVERS_EXPORT extern const QString QSQLITE;

    // Others
    TINYDRIVERS_EXPORT extern const QString NotImplemented;

} // namespace Orm::Drivers::Constants

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_DRIVERS_CONSTANTS_EXTERN_P_HPP
