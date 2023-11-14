#include "orm/drivers/mysql/mysqlutils_p.hpp"

#include "orm/drivers/macros/includemysqlh.hpp" // IWYU pragma: keep

#include "orm/drivers/sqlfield.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

using namespace Qt::StringLiterals;

namespace Orm::Drivers::MySql
{

/* public */

SqlDriverError
MySqlUtilsPrivate::makeError(const QString &error, const SqlDriverError::ErrorType type,
                             MYSQL *const mysql,
                             // CUR drivers check if this can be done also for makeStmtError() because all functions return r and then inside the makeStmtError() is the errNo obtained again silverqx
                             const std::optional<uint> errNo)
{
    const auto mysqlErrno = errNo.value_or(mysql_errno(mysql));
    const auto *const mysqlError = mysql ? mysql_error(mysql) : nullptr;

    return SqlDriverError("QMYSQL: "_L1 + error, QString::fromUtf8(mysqlError),
                          type, QString::number(mysqlErrno));
}

QMetaType
MySqlUtilsPrivate::decodeMySqlType(const enum_field_types mysqlType, const uint flags)
{
    QMetaType::Type type = QMetaType::UnknownType;

    switch (mysqlType) {
    case MYSQL_TYPE_TINY:
        type = flags & UNSIGNED_FLAG ? QMetaType::UChar : QMetaType::Char;
        break;

    case MYSQL_TYPE_SHORT:
        type = flags & UNSIGNED_FLAG ? QMetaType::UShort : QMetaType::Short;
        break;

    case MYSQL_TYPE_LONG:
    case MYSQL_TYPE_INT24:
        type = flags & UNSIGNED_FLAG ? QMetaType::UInt : QMetaType::Int;
        break;

    case MYSQL_TYPE_YEAR:
        type = QMetaType::Int;
        break;

    case MYSQL_TYPE_BIT:
    case MYSQL_TYPE_LONGLONG:
        type = flags & UNSIGNED_FLAG ? QMetaType::ULongLong : QMetaType::LongLong;
        break;

    case MYSQL_TYPE_FLOAT:
    case MYSQL_TYPE_DOUBLE:
    case MYSQL_TYPE_DECIMAL:
    case MYSQL_TYPE_NEWDECIMAL:
        type = QMetaType::Double;
        break;

    case MYSQL_TYPE_DATE:
        type = QMetaType::QDate;
        break;

    case MYSQL_TYPE_TIME:
        /* A time field can be within the range '-838:59:59' to '838:59:59' so
           use QString instead of QTime since QTime is limited to 24 hour clock. */
        type = QMetaType::QString;
        break;

    case MYSQL_TYPE_DATETIME:
    case MYSQL_TYPE_TIMESTAMP:
        type = QMetaType::QDateTime;
        break;

    case MYSQL_TYPE_STRING:
    case MYSQL_TYPE_VAR_STRING:
    case MYSQL_TYPE_BLOB:
    case MYSQL_TYPE_TINY_BLOB:
    case MYSQL_TYPE_MEDIUM_BLOB:
    case MYSQL_TYPE_LONG_BLOB:
    case MYSQL_TYPE_GEOMETRY:
    case MYSQL_TYPE_JSON:
        type = flags & BINARY_FLAG ? QMetaType::QByteArray : QMetaType::QString;
        break;

    case MYSQL_TYPE_ENUM:
    case MYSQL_TYPE_SET:
        type = QMetaType::QString;
        break;

    // Needed because there are more enum values which are not available in all headers
    default:
        type = QMetaType::QString;
        break;
    }

    return QMetaType(type);
}

bool MySqlUtilsPrivate::isInteger(const int type)
{
    return type == QMetaType::Char     || type == QMetaType::UChar  ||
           type == QMetaType::Short    || type == QMetaType::UShort ||
           type == QMetaType::Int      || type == QMetaType::UInt   ||
           type == QMetaType::LongLong || type == QMetaType::ULongLong;
}

bool MySqlUtilsPrivate::isTimeOrDate(const enum_field_types type)
{
    // BUG drivers the code that calls the qIsTimeOrDate() expect this method also matches the MYSQL_TYPE_TIME? Also check all MYSQL_TYPE_TIME occurrences silverqx
    /* Don't match the MYSQL_TYPE_TIME because its range is bigger than the QTime.
       A time field can be within the range '-838:59:59' to '838:59:59' so
       use QString instead of QTime since QTime is limited to 24 hour clock. */
    return type == MYSQL_TYPE_DATE     ||
           type == MYSQL_TYPE_DATETIME ||
           type == MYSQL_TYPE_TIMESTAMP;
}


SqlField MySqlUtilsPrivate::convertToSqlField(const MYSQL_FIELD *const fieldInfo)
{
    /* Can't use a converting constructor for this because the SqlField can't know
       anything about implementation details like the MYSQL_FIELD. */
    SqlField field(QString::fromUtf8(fieldInfo->name),
                   MySqlUtilsPrivate::decodeMySqlType(fieldInfo->type, fieldInfo->flags),
                   QString::fromUtf8(fieldInfo->table));

    field.setRequired(IS_NOT_NULL(fieldInfo->flags));
    field.setLength(fieldInfo->length);
    field.setPrecision(fieldInfo->decimals);
    field.setSqlType(fieldInfo->type);
    field.setAutoValue(fieldInfo->flags & AUTO_INCREMENT_FLAG);

    return field;
}

} // namespace Orm::Drivers::MySql

TINYORM_END_COMMON_NAMESPACE
