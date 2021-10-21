#include "orm/constants_extern.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Constants
{

    // Common chars
    const QChar SPACE      = QChar(' ');
    const QChar DOT        = QChar('.');
    const QChar SEMICOLON  = QChar(';');
    const QChar COLON      = QChar(':');
    const QChar UNDERSCORE = QChar('_');
    const QChar DASH       = QChar('-');
    const QChar MINUS      = QChar('-');
    const QChar PLUS       = QChar('+');
    const QChar ASTERISK_C = QChar('*');
    const QChar COMMA_C    = QChar(',');
    const QChar EQ_C       = QChar('=');
    const QChar NOT_C      = QChar('!');
    const QChar LT_C       = QChar('<');
    const QChar GT_C       = QChar('>');

    // Common strings
    const QString ASTERISK    = QStringLiteral("*");
    const QString COMMA       = QStringLiteral(", ");
    const QString INNER       = QStringLiteral("inner");
    const QString LEFT        = QStringLiteral("left");
    const QString RIGHT       = QStringLiteral("right");
    const QString CROSS       = QStringLiteral("cross");
    const QString ASC         = QStringLiteral("asc");
    const QString DESC        = QStringLiteral("desc");
    const QString ID          = QStringLiteral("id");
    const QString NAME        = QStringLiteral("name");
    const QString CREATED_AT  = QStringLiteral("created_at");
    const QString UPDATED_AT  = QStringLiteral("updated_at");
    const QString PARENTH_ONE = QStringLiteral("(%1)");

    const QString QMYSQL      = QStringLiteral("QMYSQL");
    const QString QPSQL       = QStringLiteral("QPSQL");
    const QString QSQLITE     = QStringLiteral("QSQLITE");
    const QString MYSQL_      = QStringLiteral("MySQL");
    const QString POSTGRESQL  = QStringLiteral("PostgreSQL");
    const QString SQLITE      = QStringLiteral("SQLite");

    const QString driver_     = QStringLiteral("driver");
    const QString host_       = QStringLiteral("host");
    const QString port_       = QStringLiteral("port");
    const QString database_   = QStringLiteral("database");
    const QString schema_     = QStringLiteral("schema");
    const QString username_   = QStringLiteral("username");
    const QString password_   = QStringLiteral("password");
    const QString charset_    = QStringLiteral("charset");
    const QString collation_  = QStringLiteral("collation");
    const QString timezone_   = QStringLiteral("timezone");
    const QString prefix_     = QStringLiteral("prefix");
    const QString options_    = QStringLiteral("options");
    const QString strict_     = QStringLiteral("strict");

    const QString isolation_level         = QStringLiteral("isolation_level");
    const QString foreign_key_constraints = QStringLiteral("foreign_key_constraints");
    const QString check_database_exists   = QStringLiteral("check_database_exists");

    const QString H127001   = QStringLiteral("127.0.0.1");
    const QString LOCALHOST = QStringLiteral("localhost");
    const QString P3306     = QStringLiteral("3306");
    const QString P5432     = QStringLiteral("5432");
    const QString ROOT      = QStringLiteral("root");
    const QString UTC       = QStringLiteral("UTC");
    const QString LOCAL     = QStringLiteral("LOCAL");
    const QString SYSTEM    = QStringLiteral("SYSTEM");
    const QString PUBLIC    = QStringLiteral("public");
    const QString UTF8      = QStringLiteral("utf8");
    const QString UTF8MB4   = QStringLiteral("utf8mb4");

    // Comparison/logical/search operators
    const QString EQ       = QStringLiteral("=");
    const QString NE       = QStringLiteral("!=");
    const QString LT       = QStringLiteral("<");
    const QString LE       = QStringLiteral("<=");
    const QString GT       = QStringLiteral(">");
    const QString GE       = QStringLiteral(">=");
    const QString OR       = QStringLiteral("or");
    const QString AND      = QStringLiteral("and");
    const QString NOT      = QStringLiteral("not");
    const QString LIKE     = QStringLiteral("like");
    const QString NLIKE    = QStringLiteral("not like");
    const QString ILIKE    = QStringLiteral("ilike");

    // Alternatives
    const QString NE_      = QStringLiteral("<>");
    const QString OR_      = QStringLiteral("||");
    const QString AND_     = QStringLiteral("&&");
    const QString NOT_     = QStringLiteral("!");

    // Bitewise operators
    const QString B_OR     = QStringLiteral("|");
    const QString B_AND    = QStringLiteral("&");

} // namespace Orm::Constants

TINYORM_END_COMMON_NAMESPACE
