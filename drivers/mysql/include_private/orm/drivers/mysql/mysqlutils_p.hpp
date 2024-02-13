#pragma once
#ifndef ORM_DRIVERS_MYSQL_MYSQLUTILS_P_HPP
#define ORM_DRIVERS_MYSQL_MYSQLUTILS_P_HPP

#include <QtGlobal>

#include <optional>

#if __has_include(<field_types.h>)
#  include <field_types.h> // MySQL header
#else
#  include "orm/drivers/mysql/macros/includemysqlh_p.hpp"
#endif

#include "orm/drivers/exceptions/sqlerror.hpp"
#include "orm/drivers/utils/notnull.hpp"

#if __has_include(<field_types.h>)
struct MYSQL;
struct MYSQL_FIELD;
struct MYSQL_STMT;
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

        /*! Alias for the MySqlErrorType. */
        using MySqlErrorType = Orm::Drivers::Exceptions::SqlError::MySqlErrorType;
        /*! Alias for the NotNull. */
        template<typename T>
        using NotNull = Orm::Drivers::Utils::NotNull<T>;

    public:
        /*! Deleted default constructor, this is a pure library class. */
        MySqlUtilsPrivate() = delete;
        /*! Deleted destructor. */
        ~MySqlUtilsPrivate() = delete;

        /* Exceptions */
        /*! Prepare MySQL errNo and error message for the SqlError exception for normal
            queries. */
        static MySqlErrorType
        prepareMySqlError(NotNull<MYSQL *> mysql,
                          std::optional<uint> errNo = std::nullopt);
        /*! Prepare MySQL errNo and error message for the SqlError exception for prepared
            queries. */
        static MySqlErrorType
        prepareStmtError(NotNull<MYSQL_STMT *> stmt);

        /* Common for both */
        /*! Decode the given MySQL field type to the Qt metatype. */
        static QMetaType decodeMySqlType(enum_field_types mysqlType, uint flags);

        /* Prepared queries */
        /*! Determine whether the given metatype ID is integer type. */
        static bool isInteger(int typeId) noexcept;
        /*! Determine whether the given MySQL field type is Date-related type. */
        static bool isTimeOrDate(enum_field_types mysqlType) noexcept;

        /* Result sets */
        /*! Convert the given MySQL field to the SqlField. */
        static SqlField convertToSqlField(const MYSQL_FIELD *fieldInfo);
    };

} // namespace MySql
} // namespace Orm::Drivers

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_DRIVERS_MYSQL_MYSQLUTILS_P_HPP
