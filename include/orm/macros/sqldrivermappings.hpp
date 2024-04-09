#pragma once
#ifndef ORM_MACROS_SQLDRIVERMAPPINGS_HPP
#define ORM_MACROS_SQLDRIVERMAPPINGS_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

/* This file provides mappings for #include-s and class types for the QtSql module and
   TinyDrivers library (our implementation of the QtSql library). All mappings are
   based on the TINYORM_USING_QTSQLDRIVERS and TINYORM_USING_TINYDRIVERS C preprocessor
   macros. There is no default, on of these macro must be defined by the build system.

   C Macro types and functions:
     - TINY_INCLUDE_TXyz macros are used in #include-s
     - TSqlXyz macros are class types with user-defined common namespace
     - TSqlXyzRaw macros are class types without any namespace (currently used
       for generating forward declarations)
     - TINY_FORWARD_DECLARE_TXyz macros are predefined forward declarations that also
       take into account the user-defined common namespace
     - TINY_FORWARD_DECLARE_TClass macro function generates forward declarations

   The following code blocks show how to use these predefined macros:

   To include and use the QSqlDatabase or Orm::Drivers::SqlDatabase:
#include <orm/macros/sqldrivermappings.hpp>
#include TINY_INCLUDE_TSqlDatabase
auto db = TSqlDatabase::addDatabase("QMYSQL");

   To forward declare QSqlQuery or Orm::Driver::SqlQuery:
#include <orm/macros/sqldrivermappings.hpp>
TINY_FORWARD_DECLARE_TSqlQuery

   or (there are predefined macros for all types, so there is no need to use it this way):
#include <orm/macros/sqldrivermappings.hpp>
TINY_FORWARD_DECLARE_TClass(TSqlQueryRaw)

*/

/* TSqlError aka. QSqlError

   TinyDrivers doesn't provide this Orm::Drivers::SqlError class, it always throws
   exceptions. There is only the Orm::Drivers::DummySqlError class, which is used
   as an empty dummy counterpart wrapper for SqlError and it does nothing.
   The TSqlError mappings aren't needed and were removed because of this. */

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
#  include <QtGlobal>
#elif defined(TINYORM_USING_TINYDRIVERS)
#  include "orm/macros/commonnamespace.hpp"
#endif

/* Main section */

// Common for both
// Types without any namespace - Exceptions
#define TQueryErrorRaw          QueryError
#define TSqlErrorRaw            SqlError
#define TSqlTransactionErrorRaw SqlTransactionError

#ifdef TINYORM_USING_QTSQLDRIVERS
// Includes
#  define TINY_INCLUDE_TSqlDatabase <QtSql/QSqlDatabase>
#  define TINY_INCLUDE_TSqlDriver   <QtSql/QSqlDriver>
#  define TINY_INCLUDE_TSqlQuery    <QtSql/QSqlQuery>
#  define TINY_INCLUDE_TSqlRecord   <QtSql/QSqlRecord>
// Includes - Exceptions
#  define TINY_INCLUDE_TQueryError          <orm/exceptions/queryerror.hpp>
#  define TINY_INCLUDE_TSqlError            <orm/exceptions/sqlerror.hpp>
#  define TINY_INCLUDE_TSqlTransactionError <orm/exceptions/sqltransactionerror.hpp>
// Types without any namespace
#  define TCursorPositionRaw  Location
#  define TPrecisionPolicyRaw NumericalPrecisionPolicy
#  define TSqlDatabaseRaw     QSqlDatabase
#  define TSqlDriverRaw       QSqlDriver
#  define TSqlQueryRaw        QSqlQuery
#  define TSqlRecordRaw       QSqlRecord
// Types with user-defined common namespace
#  define TCursorPosition  QT_PREPEND_NAMESPACE(QSql::TCursorPositionRaw)
#  define TPrecisionPolicy QT_PREPEND_NAMESPACE(QSql::TPrecisionPolicyRaw)
#  define TSqlDatabase     QT_PREPEND_NAMESPACE(TSqlDatabaseRaw)
#  define TSqlDriver       QT_PREPEND_NAMESPACE(TSqlDriverRaw)
#  define TSqlQuery        QT_PREPEND_NAMESPACE(TSqlQueryRaw)
#  define TSqlRecord       QT_PREPEND_NAMESPACE(TSqlRecordRaw)
// Types with user-defined common namespace - Exceptions
#  define TQueryError          TINYORM_PREPEND_NAMESPACE(Orm::Exceptions::TQueryErrorRaw)
#  define TSqlError            TINYORM_PREPEND_NAMESPACE(Orm::Exceptions::TSqlErrorRaw)
#  define TSqlTransactionError TINYORM_PREPEND_NAMESPACE(Orm::Exceptions::TSqlTransactionErrorRaw)

// Forward declare QtSql classes
// User namespace is not defined
#  if !defined(QT_NAMESPACE) || defined(Q_MOC_RUN)
#    define TINY_FORWARD_DECLARE_TClass(name) class name;
// User defined namespace
#  else
#    define TINY_FORWARD_DECLARE_TClass(name) \
namespace QT_NAMESPACE { class name; }
#  endif
#endif

#ifdef TINYORM_USING_TINYDRIVERS
// Includes
#  define TINY_INCLUDE_TSqlDatabase <orm/drivers/sqldatabase.hpp>
#  define TINY_INCLUDE_TSqlDriver   <orm/drivers/sqldriver.hpp>
#  define TINY_INCLUDE_TSqlQuery    <orm/drivers/sqlquery.hpp>
#  define TINY_INCLUDE_TSqlRecord   <orm/drivers/sqlrecord.hpp>
// Includes - Exceptions
#  define TINY_INCLUDE_TQueryError          <orm/drivers/exceptions/queryerror.hpp>
#  define TINY_INCLUDE_TSqlError            <orm/drivers/exceptions/sqlerror.hpp>
#  define TINY_INCLUDE_TSqlTransactionError <orm/drivers/exceptions/sqltransactionerror.hpp>
// Types without any namespace
#  define TCursorPositionRaw  CursorPosition
#  define TPrecisionPolicyRaw NumericalPrecisionPolicy
#  define TSqlDatabaseRaw     SqlDatabase
#  define TSqlDriverRaw       SqlDriver
#  define TSqlQueryRaw        SqlQuery
#  define TSqlRecordRaw       SqlRecord
// Types with user-defined common namespace
#  define TCursorPosition  TINYORM_PREPEND_NAMESPACE(Orm::Drivers::TCursorPositionRaw)
#  define TPrecisionPolicy TINYORM_PREPEND_NAMESPACE(Orm::Drivers::TPrecisionPolicyRaw)
#  define TSqlDatabase     TINYORM_PREPEND_NAMESPACE(Orm::Drivers::TSqlDatabaseRaw)
#  define TSqlDriver       TINYORM_PREPEND_NAMESPACE(Orm::Drivers::TSqlDriverRaw)
#  define TSqlQuery        TINYORM_PREPEND_NAMESPACE(Orm::Drivers::TSqlQueryRaw)
#  define TSqlRecord       TINYORM_PREPEND_NAMESPACE(Orm::Drivers::TSqlRecordRaw)
// Types with user-defined common namespace - Exceptions
#  define TQueryError          TINYORM_PREPEND_NAMESPACE(Orm::Drivers::Exceptions::TQueryErrorRaw)
#  define TSqlError            TINYORM_PREPEND_NAMESPACE(Orm::Drivers::Exceptions::TSqlErrorRaw)
#  define TSqlTransactionError TINYORM_PREPEND_NAMESPACE(Orm::Drivers::Exceptions::TSqlTransactionErrorRaw)

// Forward declare TinyDrivers classes
// User namespace is not defined
#  ifndef TINYORM_COMMON_NAMESPACE
#    define TINY_FORWARD_DECLARE_TClass(name) namespace Orm::Drivers { class name; }
// User defined namespace
#  else
#    define TINY_FORWARD_DECLARE_TClass(name) \
namespace TINYORM_COMMON_NAMESPACE::Orm::Drivers { class name; }
#  endif
#endif

/* Common for both TINYORM_USING_QTSQLDRIVERS and TINYORM_USING_TINYDRIVERS */

// All forward declared TClass-es
#define TINY_FORWARD_DECLARE_TCursorPosition  TINY_FORWARD_DECLARE_TClass(TCursorPositionRaw)
#define TINY_FORWARD_DECLARE_TPrecisionPolicy TINY_FORWARD_DECLARE_TClass(TPrecisionPolicyRaw)
#define TINY_FORWARD_DECLARE_TSqlDatabase     TINY_FORWARD_DECLARE_TClass(TSqlDatabaseRaw)
#define TINY_FORWARD_DECLARE_TSqlDriver       TINY_FORWARD_DECLARE_TClass(TSqlDriverRaw)
#define TINY_FORWARD_DECLARE_TSqlQuery        TINY_FORWARD_DECLARE_TClass(TSqlQueryRaw)
#define TINY_FORWARD_DECLARE_TSqlRecord       TINY_FORWARD_DECLARE_TClass(TSqlRecordRaw)

#endif // ORM_MACROS_SQLDRIVERMAPPINGS_HPP
