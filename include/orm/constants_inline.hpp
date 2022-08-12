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
    inline const QString SIZE        = QStringLiteral("size");
    inline const QString &SIZE_      = Orm::Constants::SIZE;
    inline const QString CREATED_AT  = QStringLiteral("created_at");
    inline const QString UPDATED_AT  = QStringLiteral("updated_at");
    inline const QString PARENTH_ONE = QStringLiteral("(%1)");
    inline const QString NEWLINE     = QStringLiteral("\n");
    inline const QString DOT_IN      = QStringLiteral("%1.%2");
    inline const QString SPACE_IN    = QStringLiteral("%1 %2");
    inline const QString NOSPACE     = QStringLiteral("%1%2");
    inline const QString EMPTY       = QLatin1String("");
    inline const QString text_       = QStringLiteral("text");
    inline const QString Prepared    = QStringLiteral("prepared");
    inline const QString Unprepared  = QStringLiteral("unprepared");
    inline const QString null_       = QStringLiteral("null");
    inline const QString Version     = QStringLiteral("version");

    inline const QString QMYSQL      = QStringLiteral("QMYSQL");
    inline const QString QPSQL       = QStringLiteral("QPSQL");
    inline const QString QSQLITE     = QStringLiteral("QSQLITE");
    inline const QString MYSQL_      = QStringLiteral("MySQL");
    inline const QString POSTGRESQL  = QStringLiteral("PostgreSQL");
    inline const QString SQLITE      = QStringLiteral("SQLite");

    inline const QString driver_     = QStringLiteral("driver");
    inline const QString host_       = QStringLiteral("host");
    inline const QString port_       = QStringLiteral("port");
    inline const QString database_   = QStringLiteral("database");
    inline const QString schema_     = QStringLiteral("schema");
    inline const QString username_   = QStringLiteral("username");
    inline const QString password_   = QStringLiteral("password");
    inline const QString charset_    = QStringLiteral("charset");
    inline const QString collation_  = QStringLiteral("collation");
    inline const QString timezone_   = QStringLiteral("timezone");
    inline const QString prefix_     = QStringLiteral("prefix");
    inline const QString options_    = QStringLiteral("options");
    inline const QString strict_     = QStringLiteral("strict");
    inline const QString engine_     = QStringLiteral("engine");
    inline const QString dont_drop   = QStringLiteral("dont_drop");

    inline const QString
    isolation_level         = QStringLiteral("isolation_level");
    inline const QString
    foreign_key_constraints = QStringLiteral("foreign_key_constraints");
    inline const QString
    check_database_exists   = QStringLiteral("check_database_exists");
    inline const QString
    prefix_indexes          = QStringLiteral("prefix_indexes");

    inline const QString H127001   = QStringLiteral("127.0.0.1");
    inline const QString LOCALHOST = QStringLiteral("localhost");
    inline const QString P3306     = QStringLiteral("3306");
    inline const QString P5432     = QStringLiteral("5432");
    inline const QString ROOT      = QStringLiteral("root");
    inline const QString UTC       = QStringLiteral("UTC");
    inline const QString LOCAL     = QStringLiteral("LOCAL");
    inline const QString SYSTEM    = QStringLiteral("SYSTEM");
    inline const QString TZ00      = QStringLiteral("+00:00");
    inline const QString PUBLIC    = QStringLiteral("public");
    inline const QString UTF8      = QStringLiteral("utf8");
    inline const QString UTF8MB4   = QStringLiteral("utf8mb4");
    inline const QString InnoDB    = QStringLiteral("InnoDB");
    inline const QString MyISAM    = QStringLiteral("MyISAM");
    inline const QString postgres_ = QStringLiteral("postgres");

    inline const QString UTF8Generalci   = QStringLiteral("utf8_general_ci");
    inline const QString UTF8Unicodeci   = QStringLiteral("utf8_unicode_ci");
    inline const QString UTF8MB40900aici = QStringLiteral("utf8mb4_0900_ai_ci");
    inline const QString UcsBasic        = QStringLiteral("ucs_basic");
    inline const QString POSIX_          = QStringLiteral("POSIX");
    inline const QString NotImplemented  = QStringLiteral("Not implemented :/.");

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
