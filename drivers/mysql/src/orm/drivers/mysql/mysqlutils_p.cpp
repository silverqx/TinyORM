#include "orm/drivers/mysql/mysqlutils_p.hpp"

#include "orm/drivers/mysql/macros/includemysqlh_p.hpp"

#include "orm/drivers/sqlfield.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

using namespace Qt::StringLiterals; // NOLINT(google-build-using-namespace)

using MySqlErrorType = Orm::Drivers::Exceptions::SqlError::MySqlErrorType;

namespace Orm::Drivers::MySql
{

/* public */

/* Exceptions */

MySqlErrorType
MySqlUtilsPrivate::prepareMySqlError(const NotNull<MYSQL *> mysql,
                                     const std::optional<uint> errNo)
{
    return {QString::number(errNo.value_or(mysql_errno(mysql))),
            // FUTURE drivers correctly support MySQL errors encoding, the QString::fromUtf8() must be changed to conversion from SET NAMES to UTF8; see https://dev.mysql.com/doc/refman/8.0/en/charset-errors.html silverqx
            QString::fromUtf8(mysql_error(mysql))};
}

MySqlErrorType MySqlUtilsPrivate::prepareStmtError(const NotNull<MYSQL_STMT *> stmt)
{
    return {QString::number(mysql_stmt_errno(stmt)),
            // FUTURE drivers correctly support MySQL errors encoding, the QString::fromUtf8() must be changed to conversion from SET NAMES to UTF8; see https://dev.mysql.com/doc/refman/8.0/en/charset-errors.html silverqx
            QString::fromUtf8(mysql_stmt_error(stmt))};
}

/* Common for both */

QMetaType
MySqlUtilsPrivate::decodeMySqlType(const enum_field_types mysqlType, const uint flags)
{
    QMetaType::Type typeId = QMetaType::UnknownType;

    switch (mysqlType) {
    case MYSQL_TYPE_TINY: // 8-bit
        typeId = (flags & UNSIGNED_FLAG) != 0U ? QMetaType::UChar : QMetaType::Char;
        break;

    case MYSQL_TYPE_SHORT: // 16-bit
        typeId = (flags & UNSIGNED_FLAG) != 0U ? QMetaType::UShort : QMetaType::Short;
        break;

    case MYSQL_TYPE_LONG:
    case MYSQL_TYPE_INT24: // 32-bit (INT24 is 24-bit) both as the int type
        typeId = (flags & UNSIGNED_FLAG) != 0U ? QMetaType::UInt : QMetaType::Int;
        break;

    case MYSQL_TYPE_YEAR:
        typeId = QMetaType::Int;
        break;

    case MYSQL_TYPE_BIT:
    case MYSQL_TYPE_LONGLONG: // 64-bit
        typeId = (flags & UNSIGNED_FLAG) != 0U ? QMetaType::ULongLong
                                               : QMetaType::LongLong;
        break;

    case MYSQL_TYPE_FLOAT:
    case MYSQL_TYPE_DOUBLE:
    case MYSQL_TYPE_DECIMAL:
    case MYSQL_TYPE_NEWDECIMAL:
        typeId = QMetaType::Double;
        break;

    case MYSQL_TYPE_DATE:
        typeId = QMetaType::QDate;
        break;

    case MYSQL_TYPE_TIME:
        /* A time field can be within the range '-838:59:59' to '838:59:59' so
           use QString instead of QTime since QTime is limited to 24 hour clock. */
        typeId = QMetaType::QString;
        break;

    case MYSQL_TYPE_DATETIME:
    case MYSQL_TYPE_TIMESTAMP:
        typeId = QMetaType::QDateTime;
        break;

    case MYSQL_TYPE_STRING:
    case MYSQL_TYPE_VAR_STRING:
    case MYSQL_TYPE_BLOB:
    case MYSQL_TYPE_TINY_BLOB:
    case MYSQL_TYPE_MEDIUM_BLOB:
    case MYSQL_TYPE_LONG_BLOB:
    case MYSQL_TYPE_GEOMETRY:
// MYSQL_TYPE_JSON was added in MySQL 5.7.8
#if defined(MYSQL_VERSION_ID) && MYSQL_VERSION_ID >= 50708
    case MYSQL_TYPE_JSON:
        typeId = (flags & BINARY_FLAG) != 0U ? QMetaType::QByteArray : QMetaType::QString;
        break;
#endif

    case MYSQL_TYPE_ENUM:
    case MYSQL_TYPE_SET:
    // Needed because there are more enum values which are not available in all headers
    default:
        typeId = QMetaType::QString;
        break;
    }

    return QMetaType(typeId);
}

/* Prepared queries */

bool MySqlUtilsPrivate::isInteger(const int typeId) noexcept
{
    return typeId == QMetaType::Char     || typeId == QMetaType::UChar  ||
           typeId == QMetaType::Short    || typeId == QMetaType::UShort ||
           typeId == QMetaType::Int      || typeId == QMetaType::UInt   ||
           typeId == QMetaType::LongLong || typeId == QMetaType::ULongLong;
}

bool MySqlUtilsPrivate::isTimeOrDate(const enum_field_types mysqlType) noexcept
{
    // BUG drivers the code that calls the qIsTimeOrDate() expect this method also matches the MYSQL_TYPE_TIME? Also check all MYSQL_TYPE_TIME occurrences silverqx
    /* Don't match the MYSQL_TYPE_TIME because its range is bigger than the QTime.
       A time field can be within the range '-838:59:59' to '838:59:59' so
       use QString instead of QTime since QTime is limited to 24 hour clock. */
    return mysqlType == MYSQL_TYPE_DATE     ||
           mysqlType == MYSQL_TYPE_DATETIME ||
           mysqlType == MYSQL_TYPE_TIMESTAMP;
}

/* Result sets */

SqlField MySqlUtilsPrivate::convertToSqlField(const MYSQL_FIELD *const fieldInfo)
{
    /* Can't use a converting constructor for this because the SqlField can't know
       anything about implementation details like the MYSQL_FIELD. */
    SqlField field(QString::fromUtf8(fieldInfo->name),
                   MySqlUtilsPrivate::decodeMySqlType(fieldInfo->type, fieldInfo->flags),
                   QString::fromUtf8(fieldInfo->table));

    field.setAutoValue((fieldInfo->flags & AUTO_INCREMENT_FLAG) != 0U);
    // CUR drivers finish field default value silverqx
//    field.setDefaultValue(QString::fromUtf8(fieldInfo->def));
    field.setLength(static_cast<SqlField::size_type>(fieldInfo->length));
    field.setPrecision(static_cast<SqlField::size_type>(fieldInfo->decimals));
    field.setRequired(IS_NOT_NULL(fieldInfo->flags));
    field.setSqlType(fieldInfo->type);

    return field;
}

} // namespace Orm::Drivers::MySql

TINYORM_END_COMMON_NAMESPACE
