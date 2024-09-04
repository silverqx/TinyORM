#pragma once
#ifndef ORM_DRIVERS_CONSTANTS_INLINE_P_HPP
#define ORM_DRIVERS_CONSTANTS_INLINE_P_HPP

#include <QString>

#include <orm/macros/commonnamespace.hpp>

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Drivers
{
    /*! Alias for the literal operator that creates a QString. */
    using Qt::StringLiterals::operator""_s;

/*! Namespace contains common chars and strings used in the TinyDrivers project. */
namespace Constants
{

    // Common chars - QChar(u'') is faster than ''_L1
    inline const QChar SPACE = QChar(u' ');
    inline const QChar DOT   = QChar(u'.');
    inline const QChar QUOTE = QChar(u'"');

    // Common strings
    inline const QString NEWLINE = u"\n"_s;
    inline const QString COMMA   = u", "_s;
    inline const QString null_   = u"null"_s;

    // Database related
    inline const QString QMYSQL  = u"QMYSQL"_s;
//  inline const QString QPSQL   = u"QPSQL"_s;
//  inline const QString QSQLITE = u"QSQLITE"_s;

    // Others
    inline const QString NotImplemented = u"Not implemented :/."_s;

} // namespace Constants
} // namespace Orm::Drivers

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_DRIVERS_CONSTANTS_INLINE_P_HPP
