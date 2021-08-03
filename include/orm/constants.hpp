#pragma once
#ifndef ORM_CONSTANTS_HPP
#define ORM_CONSTANTS_HPP

#include <QString>

#include "orm/utils/export.hpp"

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
/*! Namespace constains common chars and strings. */
namespace Orm::Constants
{

    // Common chars
    SHAREDLIB_EXPORT extern const QChar SPACE;
    SHAREDLIB_EXPORT extern const QChar DOT;
    SHAREDLIB_EXPORT extern const QChar SEMICOLON;
    SHAREDLIB_EXPORT extern const QChar COLON;
    SHAREDLIB_EXPORT extern const QChar UNDERSCORE;
    SHAREDLIB_EXPORT extern const QChar DASH;
    SHAREDLIB_EXPORT extern const QChar MINUS;
    SHAREDLIB_EXPORT extern const QChar PLUS;
    SHAREDLIB_EXPORT extern const QChar ASTERISK_C;
    SHAREDLIB_EXPORT extern const QChar COMMA_C;
    SHAREDLIB_EXPORT extern const QChar EQ_C;
    SHAREDLIB_EXPORT extern const QChar NOT_C;
    SHAREDLIB_EXPORT extern const QChar LT_C;
    SHAREDLIB_EXPORT extern const QChar GT_C;

    // Common strings
    SHAREDLIB_EXPORT extern const QString ASTERISK;
    SHAREDLIB_EXPORT extern const QString COMMA;
    SHAREDLIB_EXPORT extern const QString INNER;
    SHAREDLIB_EXPORT extern const QString LEFT;
    SHAREDLIB_EXPORT extern const QString RIGHT;
    SHAREDLIB_EXPORT extern const QString CROSS;
    SHAREDLIB_EXPORT extern const QString ASC;
    SHAREDLIB_EXPORT extern const QString DESC;
    SHAREDLIB_EXPORT extern const QString ID;
    SHAREDLIB_EXPORT extern const QString NAME;
    SHAREDLIB_EXPORT extern const QString CREATED_AT;
    SHAREDLIB_EXPORT extern const QString UPDATED_AT;
    SHAREDLIB_EXPORT extern const QString PARENTH_ONE;

    // Comparison/logical/search operators
    SHAREDLIB_EXPORT extern const QString EQ;
    SHAREDLIB_EXPORT extern const QString NE;
    SHAREDLIB_EXPORT extern const QString LT;
    SHAREDLIB_EXPORT extern const QString LE;
    SHAREDLIB_EXPORT extern const QString GT;
    SHAREDLIB_EXPORT extern const QString GE;
    SHAREDLIB_EXPORT extern const QString OR;
    SHAREDLIB_EXPORT extern const QString AND;
    SHAREDLIB_EXPORT extern const QString NOT;
    SHAREDLIB_EXPORT extern const QString LIKE;
    SHAREDLIB_EXPORT extern const QString NLIKE;
    SHAREDLIB_EXPORT extern const QString ILIKE;

    // Alternatives
    SHAREDLIB_EXPORT extern const QString NE_;
    SHAREDLIB_EXPORT extern const QString OR_;
    SHAREDLIB_EXPORT extern const QString AND_;
    SHAREDLIB_EXPORT extern const QString NOT_;

    // Bitewise operators
    SHAREDLIB_EXPORT extern const QString B_OR;
    SHAREDLIB_EXPORT extern const QString B_AND;

} // namespace Orm::Constants
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif

#endif // ORM_CONSTANTS_HPP
