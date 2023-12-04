#pragma once
#ifndef ORM_DRIVERS_CONSTANTS_INLINE_P_HPP
#define ORM_DRIVERS_CONSTANTS_INLINE_P_HPP

#include <QString>

#include <orm/macros/commonnamespace.hpp>

TINYORM_BEGIN_COMMON_NAMESPACE

/*! Namespace contains common chars and strings used in the TinyDrivers project. */
namespace Orm::Drivers::Constants
{

    // Common chars
    inline const QChar SPACE = QLatin1Char(' ');
    inline const QChar DOT   = QLatin1Char('.');
    inline const QChar QUOTE = QLatin1Char('"');

    // Common strings
    inline const QString NEWLINE = QStringLiteral("\n");
    inline const QString null_   = QStringLiteral("null");

    // Database related
    inline const QString QMYSQL  = QStringLiteral("QMYSQL");
//    inline const QString QPSQL   = QStringLiteral("QPSQL");
//    inline const QString QSQLITE = QStringLiteral("QSQLITE");

    // Others
    inline const QString NotImplemented = QStringLiteral("Not implemented :/.");

} // namespace Orm::Drivers::Constants

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_DRIVERS_CONSTANTS_INLINE_P_HPP
