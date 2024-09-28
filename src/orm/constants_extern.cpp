#include "orm/constants_extern.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Constants
{

    // Common chars - QChar(u'') is faster than ''_L1
    const QChar SPACE         = QChar::Space;
    const QChar DOT           = QChar(u'.');
    const QChar SEMICOLON     = QChar(u';');
    const QChar COLON         = QChar(u':');
    const QChar UNDERSCORE    = QChar(u'_');
    const QChar DASH          = QChar(u'-');
    const QChar MINUS         = QChar(u'-');
    const QChar PLUS          = QChar(u'+');
    const QChar ASTERISK_C    = QChar(u'*');
    const QChar COMMA_C       = QChar(u',');
    const QChar EQ_C          = QChar(u'=');
    const QChar NOT_C         = QChar(u'!');
    const QChar LT_C          = QChar(u'<');
    const QChar GT_C          = QChar(u'>');
    const QChar QUOTE         = QChar(u'"');
    const QChar SQUOTE        = QChar(u'\'');
    const QChar UnixTimestamp = QChar(u'U');
    const QChar NEWLINE_C     = QChar::LineFeed;

    // Common strings
    const QString NEWLINE       = u"\n"_s;
    const QString EMPTY         = u""_s; // This is the fastest
    const QString ASTERISK      = u"*"_s;
    const QString COMMA         = u", "_s;
    const QString INNER         = u"inner"_s;
    const QString LEFT          = u"left"_s;
    const QString RIGHT         = u"right"_s;
    const QString CROSS         = u"cross"_s;
    const QString ASC           = u"asc"_s;
    const QString DESC          = u"desc"_s;
    const QString ID            = u"id"_s;
    const QString NAME          = u"name"_s;
    const QString SIZE_         = u"size"_s;
    const QString CREATED_AT    = u"created_at"_s;
    const QString UPDATED_AT    = u"updated_at"_s;
    const QString DELETED_AT    = u"deleted_at"_s;
    const QString text_         = u"text"_s;
    const QString Prepared      = u"prepared"_s;
    const QString Unprepared    = u"unprepared"_s;
    const QString null_         = u"null"_s;
    const QString Version       = u"version"_s;
    const QString NOTE          = u"note"_s;
    const QString pivot_        = u"pivot"_s;
    const QString HASH_         = u"hash"_s;
    const QString Progress      = u"progress"_s;
    const QString AddedOn       = u"added_on"_s;
    const QString ON            = u"ON"_s;
    const QString OFF           = u"OFF"_s;
    const QString on            = u"on"_s;
    const QString off           = u"off"_s;

    // Templates
    const QString TMPL_ONE      = u"%1"_s;
    const QString PARENTH_ONE   = u"(%1)"_s;
    const QString DOT_IN        = u"%1.%2"_s;
    const QString SPACE_IN      = u"%1 %2"_s;
    const QString UNDERSCORE_IN = u"%1_%2"_s;
    const QString NOSPACE       = u"%1%2"_s;
    const QString NOSPACE3      = u"%1%2%3"_s;
    const QString TMPL_PLURAL   = u"%1s"_s;
    const QString TMPL_SQUOTES  = u"'%1'"_s;
    const QString TMPL_DQUOTES  = uR"("%1")"_s;

    // Database related
    const QString QMYSQL        = u"QMYSQL"_s;
    const QString QPSQL         = u"QPSQL"_s;
    const QString QSQLITE       = u"QSQLITE"_s;
    const QString MYSQL_        = u"MySQL"_s;
    const QString POSTGRESQL    = u"PostgreSQL"_s;
    const QString SQLITE        = u"SQLite"_s;

    const QString driver_       = u"driver"_s;
    const QString host_         = u"host"_s;
    const QString port_         = u"port"_s;
    const QString database_     = u"database"_s;
    const QString schema_       = u"schema"_s;
    const QString search_path   = u"search_path"_s;
    const QString username_     = u"username"_s;
    const QString password_     = u"password"_s;
    const QString charset_      = u"charset"_s;
    const QString collation_    = u"collation"_s;
    const QString timezone_     = u"timezone"_s;
    const QString prefix_       = u"prefix"_s;
    const QString options_      = u"options"_s;
    const QString strict_       = u"strict"_s;
    const QString engine_       = u"engine"_s;
    const QString dont_drop     = u"dont_drop"_s;
    const QString qt_timezone   = u"qt_timezone"_s;

    // MySQL SSL
    const QString SSL_CERT      = u"SSL_CERT"_s;
    const QString SSL_KEY       = u"SSL_KEY"_s;
    const QString SSL_CA        = u"SSL_CA"_s;
    const QString SSL_MODE      = u"MYSQL_OPT_SSL_MODE"_s; // QtSql MySQL doesn't support short SSL_MODE option so this long version must be used
    const QString &ssl_cert     = SSL_CERT;
    const QString &ssl_key      = SSL_KEY;
    const QString &ssl_ca       = SSL_CA;
    const QString &ssl_mode     = SSL_MODE;
    const QString Disabled      = u"DISABLED"_s;
    const QString Preferred     = u"PREFERRED"_s;
    const QString Required      = u"REQUIRED"_s;
    const QString VerifyCA      = u"VERIFY_CA"_s;

    // PostgreSQL SSL
    const QString sslmode_      = u"sslmode"_s;
    const QString sslcert       = u"sslcert"_s;
    const QString sslkey        = u"sslkey"_s;
    const QString sslrootcert   = u"sslrootcert"_s;
    const QString verify_ca     = u"verify-ca"_s;
    const QString verify_full   = u"verify-full"_s;

    // Others
    const QString isolation_level         = u"isolation_level"_s;
    const QString foreign_key_constraints = u"foreign_key_constraints"_s;
    const QString check_database_exists   = u"check_database_exists"_s;
    const QString prefix_indexes          = u"prefix_indexes"_s;
    const QString return_qdatetime        = u"return_qdatetime"_s;
    const QString application_name        = u"application_name"_s;
    const QString synchronous_commit      = u"synchronous_commit"_s;
    const QString spatial_ref_sys         = u"spatial_ref_sys"_s;

    // Database common
    const QString H127001   = u"127.0.0.1"_s;
    const QString LOCALHOST = u"localhost"_s;
    const QString P3306     = u"3306"_s;
    const QString P5432     = u"5432"_s;
    const QString ROOT      = u"root"_s;
    const QString UTC       = u"UTC"_s;
    const QString LOCAL     = u"LOCAL"_s;   // PostgreSQL time zone
    const QString DEFAULT   = u"DEFAULT"_s; // PostgreSQL time zone
    const QString SYSTEM    = u"SYSTEM"_s;  // MySQL time zone
    const QString TZ00      = u"+00:00"_s;
    const QString PUBLIC    = u"public"_s;
    const QString UTF8      = u"utf8"_s;
    const QString UTF8MB4   = u"utf8mb4"_s;
    const QString InnoDB    = u"InnoDB"_s;
    const QString MyISAM    = u"MyISAM"_s;
    const QString postgres_ = u"postgres"_s;
    const QString in_memory = u":memory:"_s;

    const QString UTF8Generalci       = u"utf8_general_ci"_s;
    const QString UTF8Unicodeci       = u"utf8_unicode_ci"_s;
    const QString UTF8MB4Unicode520ci = u"utf8mb4_unicode_520_ci"_s;
    const QString UTF8MB40900aici     = u"utf8mb4_0900_ai_ci"_s;
    const QString UTF8MB4Uca1400aici  = u"utf8mb4_uca1400_ai_ci"_s; // MariaDB 11.5
    const QString UcsBasic            = u"ucs_basic"_s;
    const QString POSIX_              = u"POSIX"_s;

    // Comparison/logical/search operators
    const QString EQ       = u"="_s;
    const QString NE       = u"!="_s;
    const QString LT       = u"<"_s;
    const QString LE       = u"<="_s;
    const QString GT       = u">"_s;
    const QString GE       = u">="_s;
    const QString OR       = u"or"_s;
    const QString AND      = u"and"_s;
    const QString NOT      = u"not"_s;
    const QString LIKE     = u"like"_s;
    const QString NLIKE    = u"not like"_s;
    const QString ILIKE    = u"ilike"_s;

    // Alternatives
    const QString NE_      = u"<>"_s;
    const QString OR_      = u"||"_s;
    const QString AND_     = u"&&"_s;
    const QString NOT_     = u"!"_s;

    // Bitwise operators
    const QString B_OR     = u"|"_s;
    const QString B_AND    = u"&"_s;

    // Others
    const QString NotImplemented    = u"Not implemented :/."_s;
    const QString dummy_NONEXISTENT = u"dummy-NON_EXISTENT"_s;

} // namespace Orm::Constants

TINYORM_END_COMMON_NAMESPACE
