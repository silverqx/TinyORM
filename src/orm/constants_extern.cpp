#include "orm/constants_extern.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Constants
{

    // Common chars
    const QChar SPACE      = QLatin1Char(' ');
    const QChar DOT        = QLatin1Char('.');
    const QChar SEMICOLON  = QLatin1Char(';');
    const QChar COLON      = QLatin1Char(':');
    const QChar UNDERSCORE = QLatin1Char('_');
    const QChar DASH       = QLatin1Char('-');
    const QChar MINUS      = QLatin1Char('-');
    const QChar PLUS       = QLatin1Char('+');
    const QChar ASTERISK_C = QLatin1Char('*');
    const QChar COMMA_C    = QLatin1Char(',');
    const QChar EQ_C       = QLatin1Char('=');
    const QChar NOT_C      = QLatin1Char('!');
    const QChar LT_C       = QLatin1Char('<');
    const QChar GT_C       = QLatin1Char('>');
    const QChar QUOTE      = QLatin1Char('"');
    const QChar SQUOTE     = QLatin1Char('\'');

    // Common strings
    const QString NEWLINE      = QStringLiteral("\n");
    const QString EMPTY        = QLatin1String("");
    const QString ASTERISK     = QStringLiteral("*");
    const QString COMMA        = QStringLiteral(", ");
    const QString INNER        = QStringLiteral("inner");
    const QString LEFT         = QStringLiteral("left");
    const QString RIGHT        = QStringLiteral("right");
    const QString CROSS        = QStringLiteral("cross");
    const QString ASC          = QStringLiteral("asc");
    const QString DESC         = QStringLiteral("desc");
    const QString ID           = QStringLiteral("id");
    const QString NAME         = QStringLiteral("name");
    const QString SIZE_        = QStringLiteral("size");
    const QString CREATED_AT   = QStringLiteral("created_at");
    const QString UPDATED_AT   = QStringLiteral("updated_at");
    const QString DELETED_AT   = QStringLiteral("deleted_at");
    const QString text_        = QStringLiteral("text");
    const QString Prepared     = QStringLiteral("prepared");
    const QString Unprepared   = QStringLiteral("unprepared");
    const QString null_        = QStringLiteral("null");
    const QString Version      = QStringLiteral("version");
    const QString NOTE         = QStringLiteral("note");
    const QString pivot_       = QStringLiteral("pivot");
    const QString HASH_        = QStringLiteral("hash");
    const QString Progress     = QStringLiteral("progress");
    const QString ON           = QStringLiteral("ON");
    const QString OFF          = QStringLiteral("OFF");
    const QString on           = QStringLiteral("on");
    const QString off          = QStringLiteral("off");

    // Templates
    const QString TMPL_ONE     = QStringLiteral("%1");
    const QString PARENTH_ONE  = QStringLiteral("(%1)");
    const QString DOT_IN       = QStringLiteral("%1.%2");
    const QString SPACE_IN     = QStringLiteral("%1 %2");
    const QString NOSPACE      = QStringLiteral("%1%2");
    const QString NOSPACE3     = QStringLiteral("%1%2%3");
    const QString TMPL_PLURAL  = QStringLiteral("%1s");
    const QString TMPL_SQUOTES = QStringLiteral("'%1'");
    const QString TMPL_DQUOTES = QStringLiteral("\"%1\"");

    // Database related
    const QString QMYSQL       = QStringLiteral("QMYSQL");
    const QString QPSQL        = QStringLiteral("QPSQL");
    const QString QSQLITE      = QStringLiteral("QSQLITE");
    const QString MYSQL_       = QStringLiteral("MySQL");
    const QString POSTGRESQL   = QStringLiteral("PostgreSQL");
    const QString SQLITE       = QStringLiteral("SQLite");

    const QString driver_      = QStringLiteral("driver");
    const QString host_        = QStringLiteral("host");
    const QString port_        = QStringLiteral("port");
    const QString database_    = QStringLiteral("database");
    const QString schema_      = QStringLiteral("schema");
    const QString search_path  = QStringLiteral("search_path");
    const QString username_    = QStringLiteral("username");
    const QString password_    = QStringLiteral("password");
    const QString charset_     = QStringLiteral("charset");
    const QString collation_   = QStringLiteral("collation");
    const QString timezone_    = QStringLiteral("timezone");
    const QString prefix_      = QStringLiteral("prefix");
    const QString options_     = QStringLiteral("options");
    const QString strict_      = QStringLiteral("strict");
    const QString engine_      = QStringLiteral("engine");
    const QString dont_drop    = QStringLiteral("dont_drop");
    const QString qt_timezone  = QStringLiteral("qt_timezone");

    // MySQL SSL
    const QString SSL_CERT     = QStringLiteral("SSL_CERT");
    const QString SSL_KEY      = QStringLiteral("SSL_KEY");
    const QString SSL_CA       = QStringLiteral("SSL_CA");
    const QString SSL_MODE     = QStringLiteral("MYSQL_OPT_SSL_MODE"); // QtSql MySQL doesn't support short SSL_MODE option so this long version must be used
    const QString &ssl_cert    = SSL_CERT;
    const QString &ssl_key     = SSL_KEY;
    const QString &ssl_ca      = SSL_CA;
    const QString &ssl_mode    = SSL_MODE;
    const QString Disabled     = QStringLiteral("DISABLED");
    const QString Preferred    = QStringLiteral("PREFERRED");
    const QString Required     = QStringLiteral("REQUIRED");
    const QString VerifyCA     = QStringLiteral("VERIFY_CA");

    // PostgreSQL SSL
    const QString sslmode_     = QStringLiteral("sslmode");
    const QString sslcert      = QStringLiteral("sslcert");
    const QString sslkey       = QStringLiteral("sslkey");
    const QString sslrootcert  = QStringLiteral("sslrootcert");
    const QString verify_ca    = QStringLiteral("verify-ca");
    const QString verify_full  = QStringLiteral("verify-full");

    // Others
    const QString isolation_level         = QStringLiteral("isolation_level");
    const QString foreign_key_constraints = QStringLiteral("foreign_key_constraints");
    const QString check_database_exists   = QStringLiteral("check_database_exists");
    const QString prefix_indexes          = QStringLiteral("prefix_indexes");
    const QString return_qdatetime        = QStringLiteral("return_qdatetime");
    const QString application_name        = QStringLiteral("application_name");
    const QString synchronous_commit      = QStringLiteral("synchronous_commit");
    const QString spatial_ref_sys         = QStringLiteral("spatial_ref_sys");

    // Database common
    const QString H127001   = QStringLiteral("127.0.0.1");
    const QString LOCALHOST = QStringLiteral("localhost");
    const QString P3306     = QStringLiteral("3306");
    const QString P5432     = QStringLiteral("5432");
    const QString ROOT      = QStringLiteral("root");
    const QString UTC       = QStringLiteral("UTC");
    const QString LOCAL     = QStringLiteral("LOCAL"); // PostgreSQL time zone
    const QString DEFAULT   = QStringLiteral("DEFAULT"); // PostgreSQL time zone
    const QString SYSTEM    = QStringLiteral("SYSTEM"); // MySQL time zone
    const QString TZ00      = QStringLiteral("+00:00");
    const QString PUBLIC    = QStringLiteral("public");
    const QString UTF8      = QStringLiteral("utf8");
    const QString UTF8MB4   = QStringLiteral("utf8mb4");
    const QString InnoDB    = QStringLiteral("InnoDB");
    const QString MyISAM    = QStringLiteral("MyISAM");
    const QString postgres_ = QStringLiteral("postgres");
    const QString in_memory = QStringLiteral(":memory:");

    const QString UTF8Generalci       = QStringLiteral("utf8_general_ci");
    const QString UTF8Unicodeci       = QStringLiteral("utf8_unicode_ci");
    const QString UTF8MB4Unicode520ci = QStringLiteral("utf8mb4_unicode_520_ci");
    const QString UTF8MB40900aici     = QStringLiteral("utf8mb4_0900_ai_ci");
    const QString UTF8MB4Uca1400aici  = QStringLiteral("utf8mb4_uca1400_ai_ci"); // MariaDB 11.5
    const QString UcsBasic            = QStringLiteral("ucs_basic");
    const QString POSIX_              = QStringLiteral("POSIX");

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

    // Bitwise operators
    const QString B_OR     = QStringLiteral("|");
    const QString B_AND    = QStringLiteral("&");

    // Others
    const QString NotImplemented    = QStringLiteral("Not implemented :/.");
    const QString dummy_NONEXISTENT = QStringLiteral("dummy-NON_EXISTENT");

} // namespace Orm::Constants

TINYORM_END_COMMON_NAMESPACE
