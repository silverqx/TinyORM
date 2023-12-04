#pragma once
#ifndef ORM_DRIVERS_MYSQL_MYSQLUTILS_P_HPP
#define ORM_DRIVERS_MYSQL_MYSQLUTILS_P_HPP

#include <QtGlobal>

#include <field_types.h> // MySQL header

#include "orm/drivers/sqldrivererror.hpp"

struct MYSQL;
struct MYSQL_FIELD;

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

        /*! Factory method to create the SqlDriverError (from MYSQL handler). */
        static SqlDriverError
        createError(const QString &error, SqlDriverError::ErrorType errorType,
                    MYSQL *mysql, std::optional<uint> errNo = std::nullopt);

        /*! Decode the given MySQL field type to the Qt metatype. */
        static QMetaType decodeMySqlType(enum_field_types mysqlType, uint flags);

        /*! Determine whether the given metatype ID is integer type. */
        static bool isInteger(int typeId);
        /*! Determine whether the given MySQL field type is Date-related type. */
        static bool isTimeOrDate(enum_field_types mysqlType);

        /*! Convert the given MySQL field to the SqlField. */
        static SqlField convertToSqlField(const MYSQL_FIELD *fieldInfo);
    };

} // namespace MySql
} // namespace Orm::Drivers

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_DRIVERS_MYSQL_MYSQLUTILS_P_HPP
