#pragma once
#ifndef ORM_DRIVERS_MYSQL_MYSQLUTILS_P_HPP
#define ORM_DRIVERS_MYSQL_MYSQLUTILS_P_HPP

#include <QtGlobal>

#if __has_include(<field_types.h>)
#  include <field_types.h> // MySQL header
#else
#  include "orm/drivers/mysql/macros/includemysqlh_p.hpp"
#endif

#include "orm/drivers/sqlerror.hpp"

#if __has_include(<field_types.h>)
struct MYSQL;
struct MYSQL_FIELD;
#endif

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Drivers
{
    class SqlField;

namespace MySql
{

    /*! MySQL driver private library class. */
    class MySqlUtilsPrivate
    {
        Q_DISABLE_COPY_MOVE(MySqlUtilsPrivate)

    public:
        /*! Deleted default constructor, this is a pure library class. */
        MySqlUtilsPrivate() = delete;
        /*! Deleted destructor. */
        ~MySqlUtilsPrivate() = delete;

        /*! Factory method to create the SqlError (from MYSQL handler). */
        static SqlError
        createError(const QString &error, SqlError::ErrorType errorType, MYSQL *mysql,
                    std::optional<uint> errNo = std::nullopt);

        /*! Decode the given MySQL field type to the Qt metatype. */
        static QMetaType decodeMySqlType(enum_field_types mysqlType, uint flags);

        /*! Determine whether the given metatype ID is integer type. */
        static bool isInteger(int typeId) noexcept;
        /*! Determine whether the given MySQL field type is Date-related type. */
        static bool isTimeOrDate(enum_field_types mysqlType) noexcept;

        /*! Convert the given MySQL field to the SqlField. */
        static SqlField convertToSqlField(const MYSQL_FIELD *fieldInfo);
    };

} // namespace MySql
} // namespace Orm::Drivers

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_DRIVERS_MYSQL_MYSQLUTILS_P_HPP
