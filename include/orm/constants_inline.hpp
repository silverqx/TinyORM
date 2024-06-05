#pragma once
#ifndef ORM_CONSTANTS_INLINE_HPP
#define ORM_CONSTANTS_INLINE_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include <QString>

#include "orm/macros/commonnamespace.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

/*! Namespace contains common chars and strings used in the TinyOrm project. */
namespace Orm::Constants
{

    // Common chars
    inline const QChar SPACE      = QLatin1Char(' ');
    inline const QChar DOT        = QLatin1Char('.');
    inline const QChar SEMICOLON  = QLatin1Char(';');
    inline const QChar COLON      = QLatin1Char(':');
    inline const QChar UNDERSCORE = QLatin1Char('_');
    inline const QChar DASH       = QLatin1Char('-');
    inline const QChar MINUS      = QLatin1Char('-');
    inline const QChar PLUS       = QLatin1Char('+');
    inline const QChar ASTERISK_C = QLatin1Char('*');
    inline const QChar COMMA_C    = QLatin1Char(',');
    inline const QChar EQ_C       = QLatin1Char('=');
    inline const QChar NOT_C      = QLatin1Char('!');
    inline const QChar LT_C       = QLatin1Char('<');
    inline const QChar GT_C       = QLatin1Char('>');
    inline const QChar QUOTE      = QLatin1Char('"');
    inline const QChar SQUOTE     = QLatin1Char('\'');

    // Common strings
    inline const QString NEWLINE      = QStringLiteral("\n");
    inline const QString EMPTY        = QLatin1String("");
    inline const QString ASTERISK     = QStringLiteral("*");
    inline const QString COMMA        = QStringLiteral(", ");
    inline const QString INNER        = QStringLiteral("inner");
    inline const QString LEFT         = QStringLiteral("left");
    inline const QString RIGHT        = QStringLiteral("right");
    inline const QString CROSS        = QStringLiteral("cross");
    inline const QString ASC          = QStringLiteral("asc");
    inline const QString DESC         = QStringLiteral("desc");
    inline const QString ID           = QStringLiteral("id");
    inline const QString NAME         = QStringLiteral("name");
    inline const QString SIZE_        = QStringLiteral("size"); // SIZE collides with a symbol in the Windows header files
    inline const QString CREATED_AT   = QStringLiteral("created_at");
    inline const QString UPDATED_AT   = QStringLiteral("updated_at");
    inline const QString DELETED_AT   = QStringLiteral("deleted_at");
    inline const QString text_        = QStringLiteral("text");
    inline const QString Prepared     = QStringLiteral("prepared");
    inline const QString Unprepared   = QStringLiteral("unprepared");
    inline const QString null_        = QStringLiteral("null");
    inline const QString Version      = QStringLiteral("version");
    inline const QString NOTE         = QStringLiteral("note");
    inline const QString pivot_       = QStringLiteral("pivot");
    inline const QString HASH_        = QStringLiteral("hash");
    inline const QString Progress     = QStringLiteral("progress");
    inline const QString ON           = QStringLiteral("ON");
    inline const QString OFF          = QStringLiteral("OFF");
    inline const QString on           = QStringLiteral("on");
    inline const QString off          = QStringLiteral("off");

    // Templates
    inline const QString TMPL_ONE     = QStringLiteral("%1");
    inline const QString PARENTH_ONE  = QStringLiteral("(%1)");
    inline const QString DOT_IN       = QStringLiteral("%1.%2");
    inline const QString SPACE_IN     = QStringLiteral("%1 %2");
    inline const QString NOSPACE      = QStringLiteral("%1%2");
    inline const QString NOSPACE3     = QStringLiteral("%1%2%3");
    inline const QString TMPL_PLURAL  = QStringLiteral("%1s");
    inline const QString TMPL_SQUOTES = QStringLiteral("'%1'");
    inline const QString TMPL_DQUOTES = QStringLiteral("\"%1\"");

    // Database related
    inline const QString QMYSQL       = QStringLiteral("QMYSQL");
    inline const QString QPSQL        = QStringLiteral("QPSQL");
    inline const QString QSQLITE      = QStringLiteral("QSQLITE");
    inline const QString MYSQL_       = QStringLiteral("MySQL");
    inline const QString POSTGRESQL   = QStringLiteral("PostgreSQL");
    inline const QString SQLITE       = QStringLiteral("SQLite");

    inline const QString driver_      = QStringLiteral("driver");
    inline const QString host_        = QStringLiteral("host");
    inline const QString port_        = QStringLiteral("port");
    inline const QString database_    = QStringLiteral("database");
    inline const QString schema_      = QStringLiteral("schema");
    inline const QString search_path  = QStringLiteral("search_path");
    inline const QString username_    = QStringLiteral("username");
    inline const QString password_    = QStringLiteral("password");
    inline const QString charset_     = QStringLiteral("charset");
    inline const QString collation_   = QStringLiteral("collation");
    inline const QString timezone_    = QStringLiteral("timezone");
    inline const QString prefix_      = QStringLiteral("prefix");
    inline const QString options_     = QStringLiteral("options");
    inline const QString strict_      = QStringLiteral("strict");
    inline const QString engine_      = QStringLiteral("engine");
    inline const QString dont_drop    = QStringLiteral("dont_drop");
    inline const QString qt_timezone  = QStringLiteral("qt_timezone");

    // MySQL SSL
    inline const QString SSL_CERT     = QStringLiteral("SSL_CERT");
    inline const QString SSL_KEY      = QStringLiteral("SSL_KEY");
    inline const QString SSL_CA       = QStringLiteral("SSL_CA");
    inline const QString SSL_MODE     = QStringLiteral("MYSQL_OPT_SSL_MODE"); // QtSql MySQL doesn't support short SSL_MODE option so this long version must be used
    inline const QString &ssl_cert    = SSL_CERT;
    inline const QString &ssl_key     = SSL_KEY;
    inline const QString &ssl_ca      = SSL_CA;
    inline const QString &ssl_mode    = SSL_MODE;
    inline const QString Disabled     = QStringLiteral("DISABLED");
    inline const QString Preferred    = QStringLiteral("PREFERRED");
    inline const QString Required     = QStringLiteral("REQUIRED");
    inline const QString VerifyCA     = QStringLiteral("VERIFY_CA");

    // PostgreSQL SSL
    inline const QString sslmode_     = QStringLiteral("sslmode");
    inline const QString sslcert      = QStringLiteral("sslcert");
    inline const QString sslkey       = QStringLiteral("sslkey");
    inline const QString sslrootcert  = QStringLiteral("sslrootcert");
    inline const QString verify_ca    = QStringLiteral("verify-ca");
    inline const QString verify_full  = QStringLiteral("verify-full");

    // Others
    inline const QString
    isolation_level         = QStringLiteral("isolation_level");
    inline const QString
    foreign_key_constraints = QStringLiteral("foreign_key_constraints");
    inline const QString
    check_database_exists   = QStringLiteral("check_database_exists");
    inline const QString
    prefix_indexes          = QStringLiteral("prefix_indexes");
    inline const QString
    return_qdatetime        = QStringLiteral("return_qdatetime");
    inline const QString
    application_name        = QStringLiteral("application_name");
    inline const QString
    synchronous_commit      = QStringLiteral("synchronous_commit");
    inline const QString
    spatial_ref_sys         = QStringLiteral("spatial_ref_sys");

    // Database common
    inline const QString H127001   = QStringLiteral("127.0.0.1");
    inline const QString LOCALHOST = QStringLiteral("localhost");
    inline const QString P3306     = QStringLiteral("3306");
    inline const QString P5432     = QStringLiteral("5432");
    inline const QString ROOT      = QStringLiteral("root");
    inline const QString UTC       = QStringLiteral("UTC");
    inline const QString LOCAL     = QStringLiteral("LOCAL"); // PostgreSQL time zone
    inline const QString DEFAULT   = QStringLiteral("DEFAULT"); // PostgreSQL time zone
    inline const QString SYSTEM    = QStringLiteral("SYSTEM"); // MySQL time zone
    inline const QString TZ00      = QStringLiteral("+00:00");
    inline const QString PUBLIC    = QStringLiteral("public");
    inline const QString UTF8      = QStringLiteral("utf8");
    inline const QString UTF8MB4   = QStringLiteral("utf8mb4");
    inline const QString InnoDB    = QStringLiteral("InnoDB");
    inline const QString MyISAM    = QStringLiteral("MyISAM");
    inline const QString postgres_ = QStringLiteral("postgres");
    inline const QString in_memory = QStringLiteral(":memory:");

    inline const QString UTF8Generalci       = QStringLiteral("utf8_general_ci");
    inline const QString UTF8Unicodeci       = QStringLiteral("utf8_unicode_ci");
    inline const QString UTF8MB4Unicode520ci = QStringLiteral("utf8mb4_unicode_520_ci");
    inline const QString UTF8MB40900aici     = QStringLiteral("utf8mb4_0900_ai_ci");
    inline const QString UTF8MB4Uca1400aici  = QStringLiteral("utf8mb4_uca1400_ai_ci"); // MariaDB 11.5
    inline const QString UcsBasic            = QStringLiteral("ucs_basic");
    inline const QString POSIX_              = QStringLiteral("POSIX");

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

    // Bitwise operators
    inline const QString B_OR  = QStringLiteral("|");
    inline const QString B_AND = QStringLiteral("&");

    // Others
    inline const QString NotImplemented    = QStringLiteral("Not implemented :/.");
    inline const QString dummy_NONEXISTENT = QStringLiteral("dummy-NON_EXISTENT");

} // namespace Orm::Constants

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_CONSTANTS_INLINE_HPP
