#pragma once
#ifndef ORM_CONSTANTS_INLINE_HPP
#define ORM_CONSTANTS_INLINE_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include <QString>

#include "orm/macros/commonnamespace.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

/*! Namespace constains common chars and strings. */
namespace Orm::Constants
{

    // Common chars
    inline const QChar SPACE      = QChar(' ');
    inline const QChar DOT        = QChar('.');
    inline const QChar SEMICOLON  = QChar(';');
    inline const QChar COLON      = QChar(':');
    inline const QChar UNDERSCORE = QChar('_');
    inline const QChar DASH       = QChar('-');
    inline const QChar MINUS      = QChar('-');
    inline const QChar PLUS       = QChar('+');
    inline const QChar ASTERISK_C = QChar('*');
    inline const QChar COMMA_C    = QChar(',');
    inline const QChar EQ_C       = QChar('=');
    inline const QChar NOT_C      = QChar('!');
    inline const QChar LT_C       = QChar('<');
    inline const QChar GT_C       = QChar('>');

    // Common strings
    inline const QString ASTERISK    = QStringLiteral("*");
    inline const QString COMMA       = QStringLiteral(", ");
    inline const QString INNER       = QStringLiteral("inner");
    inline const QString LEFT        = QStringLiteral("left");
    inline const QString RIGHT       = QStringLiteral("right");
    inline const QString CROSS       = QStringLiteral("cross");
    inline const QString ASC         = QStringLiteral("asc");
    inline const QString DESC        = QStringLiteral("desc");
    inline const QString ID          = QStringLiteral("id");
    inline const QString NAME        = QStringLiteral("name");
    inline const QString CREATED_AT  = QStringLiteral("created_at");
    inline const QString UPDATED_AT  = QStringLiteral("updated_at");
    inline const QString PARENTH_ONE = QStringLiteral("(%1)");

    // Comparison/logical/search operators
    inline const QString EQ    = QStringLiteral("=");
    inline const QString NE    = QStringLiteral("!=");
    inline const QString LT    = QStringLiteral("<");
    inline const QString LE    = QStringLiteral("<=");
    inline const QString GT    = QStringLiteral(">");
    inline const QString GE    = QStringLiteral(">=");
    inline const QString OR    = QStringLiteral("or");
    inline const QString AND   = QStringLiteral("and");
    inline const QString NOT   = QStringLiteral("not");
    inline const QString LIKE  = QStringLiteral("like");
    inline const QString NLIKE = QStringLiteral("not like");
    inline const QString ILIKE = QStringLiteral("ilike");

    // Alternatives
    inline const QString NE_   = QStringLiteral("<>");
    inline const QString OR_   = QStringLiteral("||");
    inline const QString AND_  = QStringLiteral("&&");
    inline const QString NOT_  = QStringLiteral("!");

    // Bitewise operators
    inline const QString B_OR  = QStringLiteral("|");
    inline const QString B_AND = QStringLiteral("&");

} // namespace Orm::Constants

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_CONSTANTS_INLINE_HPP
