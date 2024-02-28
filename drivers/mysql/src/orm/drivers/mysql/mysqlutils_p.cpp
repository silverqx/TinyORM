#include "orm/drivers/mysql/mysqlutils_p.hpp"

#include "orm/drivers/mysql/macros/includemysqlh_p.hpp"

#include "orm/drivers/exceptions/sqlerror.hpp"
#include "orm/drivers/sqlfield.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

using namespace Qt::StringLiterals; // NOLINT(google-build-using-namespace)

using Orm::Drivers::Exceptions::MySqlErrorType;

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
    // This method is used for result sets

    QMetaType::Type typeId = QMetaType::UnknownType;

    switch (mysqlType) {
    case MYSQL_TYPE_TINY: // 8-bit
#ifndef MARIADB_VERSION_ID
    case MYSQL_TYPE_BOOL: // Currently just a placeholder, MySQL doesn't uses this enum/type, also, MYSQL_TYPE_BOOL == MYSQL_TYPE_TINY in prepared bindings
#endif
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

    case MYSQL_TYPE_VAR_STRING:  // VARCHAR, VARBINARY
    case MYSQL_TYPE_BLOB:        // BLOB, TEXT
    case MYSQL_TYPE_STRING:      // CHAR, BINARY and ENUM, SET
// MYSQL_TYPE_JSON was added in MySQL 5.7.8
#if defined(MYSQL_VERSION_ID) && MYSQL_VERSION_ID >= 50708
    case MYSQL_TYPE_JSON:
#endif
    case MYSQL_TYPE_GEOMETRY:    // Spatial field
    case MYSQL_TYPE_VARCHAR:     // Old enum I think and is only used for backward compatibility; everywhere in MySQL code where the MYSQL_TYPE_VAR_STRING is checked, is also the MYSQL_TYPE_VARCHAR checked
    case MYSQL_TYPE_TINY_BLOB:   // TINY, MEDIUM, and LONG are not used (it will be always MYSQL_TYPE_BLOB)
    case MYSQL_TYPE_MEDIUM_BLOB:
    case MYSQL_TYPE_LONG_BLOB:
        typeId = (flags & BINARY_FLAG) != 0U ? QMetaType::QByteArray : QMetaType::QString;
        break;

#ifndef MARIADB_VERSION_ID
    // This should never happen :/
    case MYSQL_TYPE_INVALID:
        Q_ASSERT(false);
        break;
#endif

    // Needed because there are more enum values which are not available in all headers
    default:
        typeId = QMetaType::QString;
        break;
    }

    return QMetaType(typeId);
}

/* Normal queries */

bool MySqlUtilsPrivate::isNumber(const QStringView string)
{
    if (string.isEmpty())
        return false;

    const auto *const nonDigit = std::find_if(string.cbegin(), string.cend(),
                                       [](const auto &ch)
    {
        // Is not numeric == 0
        return std::isdigit(ch.toLatin1()) == 0;
    });

    return nonDigit == string.cend();
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
    /* Don't match the MYSQL_TYPE_TIME because its range is bigger than the QTime.
       A TIME field can be within the range '-838:59:59' to '838:59:59' so
       use QString instead of QTime since QTime is limited to 24 hour clock.
       The mapping from the MYSQL_TYPE_TIME to QMetaType::QString is happening
       in the decodeMySqlType() method. */
    return mysqlType == MYSQL_TYPE_DATE     ||
           mysqlType == MYSQL_TYPE_DATETIME ||
           mysqlType == MYSQL_TYPE_TIMESTAMP;
}

/* Result sets */

namespace
{
    /*! Convert MySQL field enum type to QString (used by operator<<(QDebug)). */
    QString mysqlFieldTypeToQString(const enum_field_types mysqlType, const uint flags) {
        switch (mysqlType) {
        // String and binary types (VARCHAR, VARBINARY, CHAR, BINARY, ENUM, SET)
        case MYSQL_TYPE_VARCHAR: // Old enum I think and is only used for backward compatibility; everywhere in MySQL code where the MYSQL_TYPE_VAR_STRING is checked, is also the MYSQL_TYPE_VARCHAR checked
        case MYSQL_TYPE_VAR_STRING:
            return (flags & BINARY_FLAG) != 0U ? u"VARBINARY"_s : u"VARCHAR"_s;
        case MYSQL_TYPE_STRING:
            /* ENUM and SET values are returned as strings.
               For these, check that the type value is MYSQL_TYPE_STRING and that
               the ENUM_FLAG or SET_FLAG flag is set in the flags value.
               This is also true for MariaDB, I debugged it (to verify it). */
            if ((flags & ENUM_FLAG) != 0U)
                return u"ENUM"_s;
            else if ((flags & SET_FLAG) != 0U)
                return u"SET"_s;
            else // BINARY, CHAR
                return (flags & BINARY_FLAG) != 0U ? u"BINARY"_s : u"CHAR"_s;

        // Integer types
        case MYSQL_TYPE_TINY: // 8-bit
#ifndef MARIADB_VERSION_ID
        case MYSQL_TYPE_BOOL: // Currently just a placeholder, MySQL doesn't uses this enum/type, also, MYSQL_TYPE_BOOL == MYSQL_TYPE_TINY in prepared bindings
#endif
            return u"TINYINT"_s;
        case MYSQL_TYPE_SHORT: // 16-bit
            return u"SMALLINT"_s;
        case MYSQL_TYPE_INT24: // 24-bit
            return u"MEDIUMINT"_s;
        case MYSQL_TYPE_LONG: // 32-bit
            return u"INT"_s;
        case MYSQL_TYPE_LONGLONG: // 64-bit
            return u"BIGINT"_s;

        // Date/time types
        case MYSQL_TYPE_DATE:
            return u"DATE"_s;
        case MYSQL_TYPE_DATETIME:
            return u"DATETIME"_s;
        case MYSQL_TYPE_TIME:
            return u"TIME"_s;
        case MYSQL_TYPE_TIMESTAMP:
            return u"TIMESTAMP"_s;
        case MYSQL_TYPE_YEAR:
            return u"YEAR"_s;
        case MYSQL_TYPE_NEWDATE: // Should be Internal MySQL type but I see that it's used in prepared bindings, because of this I leave it enabled
            return u"NEWDATE"_s;

        // Floating/Fixed-Point Types
        case MYSQL_TYPE_NEWDECIMAL:
        case MYSQL_TYPE_DECIMAL:
            return u"DECIMAL"_s;
        case MYSQL_TYPE_DOUBLE:
            return u"DOUBLE"_s;
        case MYSQL_TYPE_FLOAT:
            return u"FLOAT"_s;

        // Bit-Value type
        case MYSQL_TYPE_BIT: // 64-bit
            return u"BIT"_s;

        // BLOB types
        case MYSQL_TYPE_BLOB:
        case MYSQL_TYPE_TINY_BLOB: // TINY, MEDIUM, and LONG can't be determined and they are not used (it will be always MYSQL_TYPE_BLOB)
        case MYSQL_TYPE_MEDIUM_BLOB:
        case MYSQL_TYPE_LONG_BLOB:
            return (flags & BINARY_FLAG) != 0U ? u"BLOB"_s : u"TEXT"_s;

        // Other types
        case MYSQL_TYPE_GEOMETRY: // Spatial field
            return u"GEOMETRY"_s;
        case MYSQL_TYPE_JSON:
            return u"JSON"_s;
        case MYSQL_TYPE_NULL: // Will never be used (special type for prepared bindings to always set column to NULL)
            return u"NULL"_s;
#ifndef MARIADB_VERSION_ID
        case MYSQL_TYPE_INVALID:
            // Need to know about this if it happens
            Q_ASSERT_X(false, "mysqlFieldTypeToQString()",
                       "MySQL field type is MYSQL_TYPE_INVALID.");
            return u"<invalid>"_s;
#endif
        default:
            // Need to know about this if it happens
            Q_ASSERT_X(false, "mysqlFieldTypeToQString()",
                       "MySQL field type is <unknown>.");
            return u"<unknown>"_s;
        }

        Q_UNREACHABLE();
    }
} // namespace

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
    field.setSqlTypeName(mysqlFieldTypeToQString(fieldInfo->type, fieldInfo->flags));

    return field;
}

} // namespace Orm::Drivers::MySql

TINYORM_END_COMMON_NAMESPACE
