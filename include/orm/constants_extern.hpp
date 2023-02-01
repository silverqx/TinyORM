#pragma once
#ifndef ORM_CONSTANTS_EXTERN_HPP
#define ORM_CONSTANTS_EXTERN_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include <QString>

#include "orm/macros/commonnamespace.hpp"
#include "orm/macros/export.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

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
    SHAREDLIB_EXPORT extern const QChar QUOTE;

    // Common strings
    SHAREDLIB_EXPORT extern const QString NEWLINE;
    SHAREDLIB_EXPORT extern const QString EMPTY;
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
    SHAREDLIB_EXPORT extern const QString SIZE;
    SHAREDLIB_EXPORT extern const QString &SIZE_;
    SHAREDLIB_EXPORT extern const QString CREATED_AT;
    SHAREDLIB_EXPORT extern const QString UPDATED_AT;
    SHAREDLIB_EXPORT extern const QString DELETED_AT;
    SHAREDLIB_EXPORT extern const QString text_;
    SHAREDLIB_EXPORT extern const QString Prepared;
    SHAREDLIB_EXPORT extern const QString Unprepared;
    SHAREDLIB_EXPORT extern const QString null_;
    SHAREDLIB_EXPORT extern const QString Version;

    // Templates
    SHAREDLIB_EXPORT extern const QString TMPL_ONE;
    SHAREDLIB_EXPORT extern const QString PARENTH_ONE;
    SHAREDLIB_EXPORT extern const QString DOT_IN;
    SHAREDLIB_EXPORT extern const QString SPACE_IN;
    SHAREDLIB_EXPORT extern const QString NOSPACE;
    SHAREDLIB_EXPORT extern const QString TMPL_PLURAL;
    SHAREDLIB_EXPORT extern const QString TMPL_SQUOTES;
    SHAREDLIB_EXPORT extern const QString TMPL_DQUOTES;

    // Database related
    SHAREDLIB_EXPORT extern const QString QMYSQL;
    SHAREDLIB_EXPORT extern const QString QPSQL;
    SHAREDLIB_EXPORT extern const QString QSQLITE;
    SHAREDLIB_EXPORT extern const QString MYSQL_;
    SHAREDLIB_EXPORT extern const QString POSTGRESQL;
    SHAREDLIB_EXPORT extern const QString SQLITE;

    SHAREDLIB_EXPORT extern const QString driver_;
    SHAREDLIB_EXPORT extern const QString host_;
    SHAREDLIB_EXPORT extern const QString port_;
    SHAREDLIB_EXPORT extern const QString database_;
    SHAREDLIB_EXPORT extern const QString schema_;
    SHAREDLIB_EXPORT extern const QString username_;
    SHAREDLIB_EXPORT extern const QString password_;
    SHAREDLIB_EXPORT extern const QString charset_;
    SHAREDLIB_EXPORT extern const QString collation_;
    SHAREDLIB_EXPORT extern const QString timezone_;
    SHAREDLIB_EXPORT extern const QString prefix_;
    SHAREDLIB_EXPORT extern const QString options_;
    SHAREDLIB_EXPORT extern const QString strict_;
    SHAREDLIB_EXPORT extern const QString engine_;
    SHAREDLIB_EXPORT extern const QString dont_drop;
    SHAREDLIB_EXPORT extern const QString qt_timezone;

    // MySQL SSL
    SHAREDLIB_EXPORT extern const QString SSL_CERT;
    SHAREDLIB_EXPORT extern const QString SSL_KEY;
    SHAREDLIB_EXPORT extern const QString SSL_CA;

    // PostgreSQL SSL
    SHAREDLIB_EXPORT extern const QString sslmode_;
    SHAREDLIB_EXPORT extern const QString sslcert;
    SHAREDLIB_EXPORT extern const QString sslkey;
    SHAREDLIB_EXPORT extern const QString sslrootcert;
    SHAREDLIB_EXPORT extern const QString verify_ca;
    SHAREDLIB_EXPORT extern const QString verify_full;

    SHAREDLIB_EXPORT extern const QString isolation_level;
    SHAREDLIB_EXPORT extern const QString foreign_key_constraints;
    SHAREDLIB_EXPORT extern const QString check_database_exists;
    SHAREDLIB_EXPORT extern const QString prefix_indexes;
    SHAREDLIB_EXPORT extern const QString return_qdatetime;
    SHAREDLIB_EXPORT extern const QString application_name;
    SHAREDLIB_EXPORT extern const QString synchronous_commit;
    SHAREDLIB_EXPORT extern const QString spatial_ref_sys;

    SHAREDLIB_EXPORT extern const QString H127001;
    SHAREDLIB_EXPORT extern const QString LOCALHOST;
    SHAREDLIB_EXPORT extern const QString P3306;
    SHAREDLIB_EXPORT extern const QString P5432;
    SHAREDLIB_EXPORT extern const QString ROOT;
    SHAREDLIB_EXPORT extern const QString UTC;
    SHAREDLIB_EXPORT extern const QString LOCAL; // PostgreSQL time zone
    SHAREDLIB_EXPORT extern const QString DEFAULT; // PostgreSQL time zone
    SHAREDLIB_EXPORT extern const QString SYSTEM; // MySQL time zone
    SHAREDLIB_EXPORT extern const QString TZ00;
    SHAREDLIB_EXPORT extern const QString PUBLIC;
    SHAREDLIB_EXPORT extern const QString UTF8;
    SHAREDLIB_EXPORT extern const QString UTF8MB4;
    SHAREDLIB_EXPORT extern const QString InnoDB;
    SHAREDLIB_EXPORT extern const QString MyISAM;
    SHAREDLIB_EXPORT extern const QString postgres_;

    SHAREDLIB_EXPORT extern const QString UTF8MB40900aici;
    SHAREDLIB_EXPORT extern const QString UTF8Generalci;
    SHAREDLIB_EXPORT extern const QString UTF8Unicodeci;
    SHAREDLIB_EXPORT extern const QString UcsBasic;
    SHAREDLIB_EXPORT extern const QString en_US;
    SHAREDLIB_EXPORT extern const QString POSIX_;
    SHAREDLIB_EXPORT extern const QString NotImplemented;

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

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_CONSTANTS_EXTERN_HPP
