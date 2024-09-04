#pragma once
#ifndef ORM_CONSTANTS_INLINE_HPP
#define ORM_CONSTANTS_INLINE_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include <QString>

#include "orm/macros/commonnamespace.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm
{
    /*! Alias for the literal operator that creates a QLatin1Char/StringView. */
    using Qt::StringLiterals::operator""_L1;
    /*! Alias for the literal operator that creates a QString. */
    using Qt::StringLiterals::operator""_s;

/*! Namespace contains common chars and strings used in the TinyORM project. */
namespace Constants
{

    // Common chars - QChar(u'') is faster than ''_L1
    inline const QChar SPACE         = QChar(u' ');
    inline const QChar DOT           = QChar(u'.');
    inline const QChar SEMICOLON     = QChar(u';');
    inline const QChar COLON         = QChar(u':');
    inline const QChar UNDERSCORE    = QChar(u'_');
    inline const QChar DASH          = QChar(u'-');
    inline const QChar MINUS         = QChar(u'-');
    inline const QChar PLUS          = QChar(u'+');
    inline const QChar ASTERISK_C    = QChar(u'*');
    inline const QChar COMMA_C       = QChar(u',');
    inline const QChar EQ_C          = QChar(u'=');
    inline const QChar NOT_C         = QChar(u'!');
    inline const QChar LT_C          = QChar(u'<');
    inline const QChar GT_C          = QChar(u'>');
    inline const QChar QUOTE         = QChar(u'"');
    inline const QChar SQUOTE        = QChar(u'\'');
    inline const QChar UnixTimestamp = QChar(u'U');

    // Common strings
    inline const QString NEWLINE       = u"\n"_s;
    inline const QString EMPTY         = u""_s; // This is fastest
    inline const QString ASTERISK      = u"*"_s;
    inline const QString COMMA         = u", "_s;
    inline const QString INNER         = u"inner"_s;
    inline const QString LEFT          = u"left"_s;
    inline const QString RIGHT         = u"right"_s;
    inline const QString CROSS         = u"cross"_s;
    inline const QString ASC           = u"asc"_s;
    inline const QString DESC          = u"desc"_s;
    inline const QString ID            = u"id"_s;
    inline const QString NAME          = u"name"_s;
    inline const QString SIZE_         = u"size"_s; // SIZE collides with a symbol in the Windows header files
    inline const QString CREATED_AT    = u"created_at"_s;
    inline const QString UPDATED_AT    = u"updated_at"_s;
    inline const QString DELETED_AT    = u"deleted_at"_s;
    inline const QString text_         = u"text"_s;
    inline const QString Prepared      = u"prepared"_s;
    inline const QString Unprepared    = u"unprepared"_s;
    inline const QString null_         = u"null"_s;
    inline const QString Version       = u"version"_s;
    inline const QString NOTE          = u"note"_s;
    inline const QString pivot_        = u"pivot"_s;
    inline const QString HASH_         = u"hash"_s;
    inline const QString Progress      = u"progress"_s;
    inline const QString AddedOn       = u"added_on"_s;
    inline const QString ON            = u"ON"_s;
    inline const QString OFF           = u"OFF"_s;
    inline const QString on            = u"on"_s;
    inline const QString off           = u"off"_s;

    // Templates
    inline const QString TMPL_ONE      = u"%1"_s;
    inline const QString PARENTH_ONE   = u"(%1)"_s;
    inline const QString DOT_IN        = u"%1.%2"_s;
    inline const QString SPACE_IN      = u"%1 %2"_s;
    inline const QString UNDERSCORE_IN = u"%1_%2"_s;
    inline const QString NOSPACE       = u"%1%2"_s;
    inline const QString NOSPACE3      = u"%1%2%3"_s;
    inline const QString TMPL_PLURAL   = u"%1s"_s;
    inline const QString TMPL_SQUOTES  = u"'%1'"_s;
    inline const QString TMPL_DQUOTES  = u"\"%1\""_s;

    // Database related
    inline const QString QMYSQL        = u"QMYSQL"_s;
    inline const QString QPSQL         = u"QPSQL"_s;
    inline const QString QSQLITE       = u"QSQLITE"_s;
    inline const QString MYSQL_        = u"MySQL"_s;
    inline const QString POSTGRESQL    = u"PostgreSQL"_s;
    inline const QString SQLITE        = u"SQLite"_s;

    inline const QString driver_       = u"driver"_s;
    inline const QString host_         = u"host"_s;
    inline const QString port_         = u"port"_s;
    inline const QString database_     = u"database"_s;
    inline const QString schema_       = u"schema"_s;
    inline const QString search_path   = u"search_path"_s;
    inline const QString username_     = u"username"_s;
    inline const QString password_     = u"password"_s;
    inline const QString charset_      = u"charset"_s;
    inline const QString collation_    = u"collation"_s;
    inline const QString timezone_     = u"timezone"_s;
    inline const QString prefix_       = u"prefix"_s;
    inline const QString options_      = u"options"_s;
    inline const QString strict_       = u"strict"_s;
    inline const QString engine_       = u"engine"_s;
    inline const QString dont_drop     = u"dont_drop"_s;
    inline const QString qt_timezone   = u"qt_timezone"_s;

    // MySQL SSL
    inline const QString SSL_CERT      = u"SSL_CERT"_s;
    inline const QString SSL_KEY       = u"SSL_KEY"_s;
    inline const QString SSL_CA        = u"SSL_CA"_s;
    inline const QString SSL_MODE      = u"MYSQL_OPT_SSL_MODE"_s; // QtSql MySQL doesn't support short SSL_MODE option so this long version must be used
    inline const QString &ssl_cert     = SSL_CERT;
    inline const QString &ssl_key      = SSL_KEY;
    inline const QString &ssl_ca       = SSL_CA;
    inline const QString &ssl_mode     = SSL_MODE;
    inline const QString Disabled      = u"DISABLED"_s;
    inline const QString Preferred     = u"PREFERRED"_s;
    inline const QString Required      = u"REQUIRED"_s;
    inline const QString VerifyCA      = u"VERIFY_CA"_s;

    // PostgreSQL SSL
    inline const QString sslmode_      = u"sslmode"_s;
    inline const QString sslcert       = u"sslcert"_s;
    inline const QString sslkey        = u"sslkey"_s;
    inline const QString sslrootcert   = u"sslrootcert"_s;
    inline const QString verify_ca     = u"verify-ca"_s;
    inline const QString verify_full   = u"verify-full"_s;

    // Others
    inline const QString
    isolation_level         = u"isolation_level"_s;
    inline const QString
    foreign_key_constraints = u"foreign_key_constraints"_s;
    inline const QString
    check_database_exists   = u"check_database_exists"_s;
    inline const QString
    prefix_indexes          = u"prefix_indexes"_s;
    inline const QString
    return_qdatetime        = u"return_qdatetime"_s;
    inline const QString
    application_name        = u"application_name"_s;
    inline const QString
    synchronous_commit      = u"synchronous_commit"_s;
    inline const QString
    spatial_ref_sys         = u"spatial_ref_sys"_s;

    // Database common
    inline const QString H127001   = u"127.0.0.1"_s;
    inline const QString LOCALHOST = u"localhost"_s;
    inline const QString P3306     = u"3306"_s;
    inline const QString P5432     = u"5432"_s;
    inline const QString ROOT      = u"root"_s;
    inline const QString UTC       = u"UTC"_s;
    inline const QString LOCAL     = u"LOCAL"_s;   // PostgreSQL time zone
    inline const QString DEFAULT   = u"DEFAULT"_s; // PostgreSQL time zone
    inline const QString SYSTEM    = u"SYSTEM"_s;  // MySQL time zone
    inline const QString TZ00      = u"+00:00"_s;
    inline const QString PUBLIC    = u"public"_s;
    inline const QString UTF8      = u"utf8"_s;
    inline const QString UTF8MB4   = u"utf8mb4"_s;
    inline const QString InnoDB    = u"InnoDB"_s;
    inline const QString MyISAM    = u"MyISAM"_s;
    inline const QString postgres_ = u"postgres"_s;
    inline const QString in_memory = u":memory:"_s;

    inline const QString UTF8Generalci       = u"utf8_general_ci"_s;
    inline const QString UTF8Unicodeci       = u"utf8_unicode_ci"_s;
    inline const QString UTF8MB4Unicode520ci = u"utf8mb4_unicode_520_ci"_s;
    inline const QString UTF8MB40900aici     = u"utf8mb4_0900_ai_ci"_s;
    inline const QString UTF8MB4Uca1400aici  = u"utf8mb4_uca1400_ai_ci"_s; // MariaDB 11.5
    inline const QString UcsBasic            = u"ucs_basic"_s;
    inline const QString POSIX_              = u"POSIX"_s;

    // Comparison/logical/search operators
    inline const QString EQ    = u"="_s;
    inline const QString NE    = u"!="_s;
    inline const QString LT    = u"<"_s;
    inline const QString LE    = u"<="_s;
    inline const QString GT    = u">"_s;
    inline const QString GE    = u">="_s;
    inline const QString OR    = u"or"_s;
    inline const QString AND   = u"and"_s;
    inline const QString NOT   = u"not"_s;
    inline const QString LIKE  = u"like"_s;
    inline const QString NLIKE = u"not like"_s;
    inline const QString ILIKE = u"ilike"_s;

    // Alternatives
    inline const QString NE_   = u"<>"_s;
    inline const QString OR_   = u"||"_s;
    inline const QString AND_  = u"&&"_s;
    inline const QString NOT_  = u"!"_s;

    // Bitwise operators
    inline const QString B_OR  = u"|"_s;
    inline const QString B_AND = u"&"_s;

    // Others
    inline const QString NotImplemented    = u"Not implemented :/."_s;
    inline const QString dummy_NONEXISTENT = u"dummy-NON_EXISTENT"_s;

} // namespace Constants
} // namespace Orm

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_CONSTANTS_INLINE_HPP
