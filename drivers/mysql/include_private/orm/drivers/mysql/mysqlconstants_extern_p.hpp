#pragma once
#ifndef ORM_DRIVERS_MYSQL_MYSQLCONSTANTS_EXTERN_P_HPP
#define ORM_DRIVERS_MYSQL_MYSQLCONSTANTS_EXTERN_P_HPP

#include <QString>

#include <orm/macros/commonnamespace.hpp>

TINYORM_BEGIN_COMMON_NAMESPACE

/*! Namespace contains common chars and strings used in the TinyMySql project. */
namespace Orm::Drivers::MySql::Constants
{

    // Common chars
    extern const QChar SEMICOLON;
    extern const QChar COLON;
    extern const QChar DASH;
    extern const QChar EQ_C;

    // Common strings
    extern const QString EMPTY;
    extern const QString COMMA;

    // Database related
    extern const QString QMYSQL;
//    extern const QString QPSQL;
//    extern const QString QSQLITE;

} // namespace Orm::Drivers::MySql::Constants

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_DRIVERS_MYSQL_MYSQLCONSTANTS_EXTERN_P_HPP
