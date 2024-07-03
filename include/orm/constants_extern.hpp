#pragma once
#ifndef ORM_CONSTANTS_EXTERN_HPP
#define ORM_CONSTANTS_EXTERN_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include <QString>

#include "orm/macros/commonnamespace.hpp"
#include "orm/macros/export.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

/*! Namespace contains common chars and strings used in the TinyOrm project. */
namespace Orm::Constants
{

    // Common chars
    TINYORM_EXPORT extern const QChar SPACE;
    TINYORM_EXPORT extern const QChar DOT;
    TINYORM_EXPORT extern const QChar SEMICOLON;
    TINYORM_EXPORT extern const QChar COLON;
    TINYORM_EXPORT extern const QChar UNDERSCORE;
    TINYORM_EXPORT extern const QChar DASH;
    TINYORM_EXPORT extern const QChar MINUS;
    TINYORM_EXPORT extern const QChar PLUS;
    TINYORM_EXPORT extern const QChar ASTERISK_C;
    TINYORM_EXPORT extern const QChar COMMA_C;
    TINYORM_EXPORT extern const QChar EQ_C;
    TINYORM_EXPORT extern const QChar NOT_C;
    TINYORM_EXPORT extern const QChar LT_C;
    TINYORM_EXPORT extern const QChar GT_C;
    TINYORM_EXPORT extern const QChar QUOTE;
    TINYORM_EXPORT extern const QChar SQUOTE;

    // Common strings
    TINYORM_EXPORT extern const QString NEWLINE;
    TINYORM_EXPORT extern const QString EMPTY;
    TINYORM_EXPORT extern const QString ASTERISK;
    TINYORM_EXPORT extern const QString COMMA;
    TINYORM_EXPORT extern const QString INNER;
    TINYORM_EXPORT extern const QString LEFT;
    TINYORM_EXPORT extern const QString RIGHT;
    TINYORM_EXPORT extern const QString CROSS;
    TINYORM_EXPORT extern const QString ASC;
    TINYORM_EXPORT extern const QString DESC;
    TINYORM_EXPORT extern const QString ID;
    TINYORM_EXPORT extern const QString NAME;
    TINYORM_EXPORT extern const QString SIZE_; // SIZE collides with a symbol in the Windows header files
    TINYORM_EXPORT extern const QString CREATED_AT;
    TINYORM_EXPORT extern const QString UPDATED_AT;
    TINYORM_EXPORT extern const QString DELETED_AT;
    TINYORM_EXPORT extern const QString text_;
    TINYORM_EXPORT extern const QString Prepared;
    TINYORM_EXPORT extern const QString Unprepared;
    TINYORM_EXPORT extern const QString null_;
    TINYORM_EXPORT extern const QString Version;
    TINYORM_EXPORT extern const QString NOTE;
    TINYORM_EXPORT extern const QString pivot_;
    TINYORM_EXPORT extern const QString HASH_;
    TINYORM_EXPORT extern const QString Progress;
    TINYORM_EXPORT extern const QString ON;
    TINYORM_EXPORT extern const QString OFF;
    TINYORM_EXPORT extern const QString on;
    TINYORM_EXPORT extern const QString off;

    // Templates
    TINYORM_EXPORT extern const QString TMPL_ONE;
    TINYORM_EXPORT extern const QString PARENTH_ONE;
    TINYORM_EXPORT extern const QString DOT_IN;
    TINYORM_EXPORT extern const QString SPACE_IN;
    TINYORM_EXPORT extern const QString NOSPACE;
    TINYORM_EXPORT extern const QString NOSPACE3;
    TINYORM_EXPORT extern const QString TMPL_PLURAL;
    TINYORM_EXPORT extern const QString TMPL_SQUOTES;
    TINYORM_EXPORT extern const QString TMPL_DQUOTES;

    // Database related
    TINYORM_EXPORT extern const QString QMYSQL;
    TINYORM_EXPORT extern const QString QPSQL;
    TINYORM_EXPORT extern const QString QSQLITE;
    TINYORM_EXPORT extern const QString MYSQL_;
    TINYORM_EXPORT extern const QString POSTGRESQL;
    TINYORM_EXPORT extern const QString SQLITE;

    TINYORM_EXPORT extern const QString driver_;
    TINYORM_EXPORT extern const QString host_;
    TINYORM_EXPORT extern const QString port_;
    TINYORM_EXPORT extern const QString database_;
    TINYORM_EXPORT extern const QString schema_;
    TINYORM_EXPORT extern const QString search_path;
    TINYORM_EXPORT extern const QString username_;
    TINYORM_EXPORT extern const QString password_;
    TINYORM_EXPORT extern const QString charset_;
    TINYORM_EXPORT extern const QString collation_;
    TINYORM_EXPORT extern const QString timezone_;
    TINYORM_EXPORT extern const QString prefix_;
    TINYORM_EXPORT extern const QString options_;
    TINYORM_EXPORT extern const QString strict_;
    TINYORM_EXPORT extern const QString engine_;
    TINYORM_EXPORT extern const QString dont_drop;
    TINYORM_EXPORT extern const QString qt_timezone;

    // MySQL SSL
    TINYORM_EXPORT extern const QString SSL_CERT;
    TINYORM_EXPORT extern const QString SSL_KEY;
    TINYORM_EXPORT extern const QString SSL_CA;
    TINYORM_EXPORT extern const QString SSL_MODE;
    TINYORM_EXPORT extern const QString &ssl_cert;
    TINYORM_EXPORT extern const QString &ssl_key;
    TINYORM_EXPORT extern const QString &ssl_ca;
    TINYORM_EXPORT extern const QString &ssl_mode;
    TINYORM_EXPORT extern const QString Disabled;
    TINYORM_EXPORT extern const QString Preferred;
    TINYORM_EXPORT extern const QString Required;
    TINYORM_EXPORT extern const QString VerifyCA;

    // PostgreSQL SSL
    TINYORM_EXPORT extern const QString sslmode_;
    TINYORM_EXPORT extern const QString sslcert;
    TINYORM_EXPORT extern const QString sslkey;
    TINYORM_EXPORT extern const QString sslrootcert;
    TINYORM_EXPORT extern const QString verify_ca;
    TINYORM_EXPORT extern const QString verify_full;

    // Others
    TINYORM_EXPORT extern const QString isolation_level;
    TINYORM_EXPORT extern const QString foreign_key_constraints;
    TINYORM_EXPORT extern const QString check_database_exists;
    TINYORM_EXPORT extern const QString prefix_indexes;
    TINYORM_EXPORT extern const QString return_qdatetime;
    TINYORM_EXPORT extern const QString application_name;
    TINYORM_EXPORT extern const QString synchronous_commit;
    TINYORM_EXPORT extern const QString spatial_ref_sys;

    // Database common
    TINYORM_EXPORT extern const QString H127001;
    TINYORM_EXPORT extern const QString LOCALHOST;
    TINYORM_EXPORT extern const QString P3306;
    TINYORM_EXPORT extern const QString P5432;
    TINYORM_EXPORT extern const QString ROOT;
    TINYORM_EXPORT extern const QString UTC;
    TINYORM_EXPORT extern const QString LOCAL; // PostgreSQL time zone
    TINYORM_EXPORT extern const QString DEFAULT; // PostgreSQL time zone
    TINYORM_EXPORT extern const QString SYSTEM; // MySQL time zone
    TINYORM_EXPORT extern const QString TZ00;
    TINYORM_EXPORT extern const QString PUBLIC;
    TINYORM_EXPORT extern const QString UTF8;
    TINYORM_EXPORT extern const QString UTF8MB4;
    TINYORM_EXPORT extern const QString InnoDB;
    TINYORM_EXPORT extern const QString MyISAM;
    TINYORM_EXPORT extern const QString postgres_;
    TINYORM_EXPORT extern const QString in_memory;

    TINYORM_EXPORT extern const QString UTF8Generalci;
    TINYORM_EXPORT extern const QString UTF8Unicodeci;
    TINYORM_EXPORT extern const QString UTF8MB4Unicode520ci;
    TINYORM_EXPORT extern const QString UTF8MB40900aici;
    TINYORM_EXPORT extern const QString UTF8MB4Uca1400aici;
    TINYORM_EXPORT extern const QString UcsBasic;
    TINYORM_EXPORT extern const QString POSIX_;

    // Comparison/logical/search operators
    TINYORM_EXPORT extern const QString EQ;
    TINYORM_EXPORT extern const QString NE;
    TINYORM_EXPORT extern const QString LT;
    TINYORM_EXPORT extern const QString LE;
    TINYORM_EXPORT extern const QString GT;
    TINYORM_EXPORT extern const QString GE;
    TINYORM_EXPORT extern const QString OR;
    TINYORM_EXPORT extern const QString AND;
    TINYORM_EXPORT extern const QString NOT;
    TINYORM_EXPORT extern const QString LIKE;
    TINYORM_EXPORT extern const QString NLIKE;
    TINYORM_EXPORT extern const QString ILIKE;

    // Alternatives
    TINYORM_EXPORT extern const QString NE_;
    TINYORM_EXPORT extern const QString OR_;
    TINYORM_EXPORT extern const QString AND_;
    TINYORM_EXPORT extern const QString NOT_;

    // Bitwise operators
    TINYORM_EXPORT extern const QString B_OR;
    TINYORM_EXPORT extern const QString B_AND;

    // Others
    TINYORM_EXPORT extern const QString NotImplemented;
    TINYORM_EXPORT extern const QString dummy_NONEXISTENT;

} // namespace Orm::Constants

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_CONSTANTS_EXTERN_HPP
