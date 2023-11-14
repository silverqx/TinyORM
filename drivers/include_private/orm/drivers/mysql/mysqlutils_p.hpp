#pragma once
#ifndef ORM_DRIVERS_MYSQL_MYSQLUTILS_P_HPP
#define ORM_DRIVERS_MYSQL_MYSQLUTILS_P_HPP

#include <orm/macros/systemheader.hpp>
TINY_SYSTEM_HEADER

#include <QtGlobal>

#include <field_types.h>

#include "orm/drivers/sqldrivererror.hpp"

struct MYSQL;
struct MYSQL_FIELD;

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Drivers
{
    class SqlField;

namespace MySql
{

    class MySqlDriverPrivate;

    /*! MySQL driver private library class. */
    class SHAREDLIB_EXPORT MySqlUtilsPrivate
    {
        Q_DISABLE_COPY_MOVE(MySqlUtilsPrivate)

    public:
        /*! Deleted default constructor, this is a pure library class. */
        MySqlUtilsPrivate() = delete;
        /*! Deleted destructor. */
        ~MySqlUtilsPrivate() = delete;

        static SqlDriverError
        makeError(const QString &error, SqlDriverError::ErrorType type,
                  MYSQL *mysql, std::optional<uint> errNo = std::nullopt);

        static QMetaType decodeMySqlType(enum_field_types mysqltype, uint flags);

        static bool isInteger(int type);
        static bool isTimeOrDate(enum_field_types type);
        static SqlField convertToSqlField(const MYSQL_FIELD *fieldInfo);
    };

} // namespace MySql
} // namespace Orm::Drivers

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_DRIVERS_MYSQL_MYSQLUTILS_P_HPP
