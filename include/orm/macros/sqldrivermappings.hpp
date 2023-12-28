#pragma once
#ifndef ORM_MACROS_SQLDRIVERMAPPINGS_HPP
#define ORM_MACROS_SQLDRIVERMAPPINGS_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

/*
   How SQL drivers mapping work.
   eg.:
#include <orm/macros/sqldrivermappings.hpp>
#include <TINY_INCLUDE_TSqlDatabase>
auto db = TSqlDatabase::addDatabase("QMYSQL");
*/

/* Checks */

#if defined(TINYORM_USING_QTSQLDRIVERS) && defined(TINYORM_USING_TINYDRIVERS)
#  error Both TINYORM_USING_QTSQLDRIVERS and TINYORM_USING_TINYDRIVERS defined.
#endif

#if !defined(TINYORM_USING_QTSQLDRIVERS) && !defined(TINYORM_USING_TINYDRIVERS)
#  error Please define the TINYORM_USING_QTSQLDRIVERS or TINYORM_USING_TINYDRIVERS \
C preprocessor macro to select the SQL drivers to use.
#endif

/* Includes */

#ifdef TINYORM_USING_QTSQLDRIVERS
#  include <QtCore/qtconfigmacros.h>
#elif defined(TINYORM_USING_TINYDRIVERS)
#  include "orm/macros/commonnamespace.hpp"
#else
#  error
#endif

/* Main section */

#ifdef TINYORM_USING_QTSQLDRIVERS
// Includes
#  define TINY_INCLUDE_TSqlDatabase QtSql/QSqlDatabase
#  define TINY_INCLUDE_TSqlDriver   QtSql/QSqlDriver
#  define TINY_INCLUDE_TSqlError    QtSql/QSqlError
#  define TINY_INCLUDE_TSqlQuery    QtSql/QSqlQuery
#  define TINY_INCLUDE_TSqlRecord   QtSql/QSqlRecord
// Types
#  define TSqlDatabase QT_PREPEND_NAMESPACE(QSqlDatabase)
#  define TSqlDriver   QT_PREPEND_NAMESPACE(QSqlDriver)
#  define TSqlError    QT_PREPEND_NAMESPACE(QSqlError)
#  define TSqlQuery    QT_PREPEND_NAMESPACE(QSqlQuery)
#  define TSqlRecord   QT_PREPEND_NAMESPACE(QSqlRecord)
#endif

#ifdef TINYORM_USING_TINYDRIVERS
// Includes
#  define TINY_INCLUDE_TSqlDatabase orm/drivers/sqldatabase.hpp
#  define TINY_INCLUDE_TSqlDriver   orm/drivers/sqldriver.hpp
#  define TINY_INCLUDE_TSqlError    orm/drivers/sqlerror.hpp
#  define TINY_INCLUDE_TSqlQuery    orm/drivers/sqlquery.hpp
#  define TINY_INCLUDE_TSqlRecord   orm/drivers/sqlrecord.hpp
// Types
#  define TSqlDatabase TINYORM_PREPEND_NAMESPACE(Orm::Drivers::SqlDatabase)
#  define TSqlDriver   TINYORM_PREPEND_NAMESPACE(Orm::Drivers::SqlDriver)
#  define TSqlError    TINYORM_PREPEND_NAMESPACE(Orm::Drivers::SqlError)
#  define TSqlQuery    TINYORM_PREPEND_NAMESPACE(Orm::Drivers::SqlQuery)
#  define TSqlRecord   TINYORM_PREPEND_NAMESPACE(Orm::Drivers::SqlRecord)
#endif

#endif // ORM_MACROS_SQLDRIVERMAPPINGS_HPP
