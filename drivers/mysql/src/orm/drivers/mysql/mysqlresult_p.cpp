#include "orm/drivers/mysql/mysqlresult_p.hpp"

#include <QDateTime>

#include "orm/drivers/exceptions/outofrangeerror.hpp"
#include "orm/drivers/mysql/mysqlconstants_p.hpp"
#include "orm/drivers/mysql/mysqlutils_p.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

using namespace Qt::StringLiterals; // NOLINT(google-build-using-namespace)

using Orm::Drivers::MySql::Constants::COLON;
using Orm::Drivers::MySql::Constants::DASH;


using MySqlUtils = Orm::Drivers::MySql::MySqlUtilsPrivate;

namespace Orm::Drivers::MySql
{

/* public */

/* Normal queries */

bool MySqlResultPrivate::populateFields(MYSQL *const mysql)
{
    const auto fieldsCount = mysql_field_count(mysql);
    const auto hasFields = fieldsCount > 0;

    // Nothing to populate, no result set
    if (!hasFields)
        return false;

    /* MariaDB doesn't have the mysql_result_metadata() function (metadata for normal
       queries). */
#ifndef MARIADB_VERSION_ID
    /* This should never happen because MYSQL_OPT_OPTIONAL_RESULTSET_METADATA and
       CLIENT_OPTIONAL_RESULTSET_METADATA aren't allowed for MySQL connections. */
    // Nothing to do, metadata disabled
    if (mysql_result_metadata(result) == RESULTSET_METADATA_NONE)
        throw Exceptions::LogicError(
                u"Result set has no metadata in %1()."_s.arg(__tiny_func__));
#endif

    uint index = 0;
    const MYSQL_FIELD *fieldInfo = nullptr;
    resultFields.resize(fieldsCount);
                       // Returns no errors
    while((fieldInfo = mysql_fetch_field(result)) != nullptr) {
        resultFields[index].metaType = MySqlUtils::decodeMySqlType(fieldInfo->type,
                                                                   fieldInfo->flags);
        resultFields[index].myField = fieldInfo;

        ++index;
    }

    // Executed query has result set
    return hasFields && wasAllFieldsFetched(fieldsCount, index, "populateFields"_L1);
}

/* Prepared queries */

bool MySqlResultPrivate::bindResultValues()
{
    // Obtain the Result Set metadata (nothing to do if no metadata)
    if (meta = mysql_stmt_result_metadata(stmt); meta == nullptr)
        /* Don't log a warning about no metadata here as the INSERT, UPDATE, and DELETE
           queries have no metadata. */
        return false;

    const auto fieldsCount = mysql_num_fields(meta);
    const auto hasFields = fieldsCount > 0;

    // Nothing to populate, no result set
    if (!hasFields)
        return false;

    /* Prepared queries don't use metadata the same way as normal queries,
       it's always RESULTSET_METADATA_NONE. */

    // Allocate memory for result sets that will be obtained from the database
    allocateMemoryForBindings(resultBinds, fieldsCount);

    uint index = 0;
    const MYSQL_FIELD *fieldInfo = nullptr;
    resultFields.resize(fieldsCount);
                       // Returns no errors
    while((fieldInfo = mysql_fetch_field(meta)) != nullptr) {
        auto &resultBind = resultBinds[index];
        auto &field = resultFields[index];

        field.myField = fieldInfo;
        field.metaType = MySqlUtils::decodeMySqlType(fieldInfo->type, fieldInfo->flags);
        resultBind.buffer_type = fieldInfo->type;

        if (isBlobType(fieldInfo->type)) {
            /* The size of a blob-field is available as soon as
               the mysql_stmt_store_result() is called, it's after
               the mysql_stmt_exec() in MySqlResult::exec(). */
            resultBind.buffer_length = field.fieldValueSize = 0UL;
            hasBlobs = true;
        }
        else if (MySqlUtils::isTimeOrDate(fieldInfo->type))
            resultBind.buffer_length = field.fieldValueSize = sizeof (MYSQL_TIME);

        else if (MySqlUtils::isInteger(field.metaType.id()))
            resultBind.buffer_length = field.fieldValueSize = 8UL;
        else {
            resultBind.buffer_type = MYSQL_TYPE_STRING;
            // CUR drivers I didn't see +1 for NULL char anywhere on any MySQL docs page in any example, try it with full buffers for more type like char[40] or varchar[255] and find out how it works and behaves silverqx
            // +1 for the NULL character
            resultBind.buffer_length = field.fieldValueSize = fieldInfo->length + 1UL;
        }

        // The following two lines only bind data members using pointers (no real values)
        resultBind.is_null     = &field.isNull;
        resultBind.length      = &field.fieldValueSize;
        resultBind.is_unsigned = (fieldInfo->flags & UNSIGNED_FLAG) != 0U;

        /* Prepare the output/result buffer (it has nothing to do with prepared bindings),
           +1 for the terminating null character. */
        field.fieldValue = resultBind.buffer_length > 0UL
                           ? std::make_unique<char[]>(resultBind.buffer_length + 1UL) // NOLINT(modernize-avoid-c-arrays)
                           : nullptr;
        resultBind.buffer = static_cast<void *>(field.fieldValue.get());

        ++index;
    }

    // Executed query has result set
    return hasFields && wasAllFieldsFetched(fieldsCount, index, "bindResultValues"_L1);
}

bool MySqlResultPrivate::shouldPrepareBindings(const uint placeholdersCount) const
{
    const auto boundValuesSize = static_cast<ulong>(boundValues.size());

    // Check prepared bindings count and show warnings
    checkPreparedBindingsCount(placeholdersCount, boundValuesSize);

    return placeholdersCount > 0 && placeholdersCount <= boundValuesSize;
}

void MySqlResultPrivate::checkPreparedBindingsCount(const uint placeholdersCount,
                                                    const ulong valuesSize)
{
    // This is more likely an info message, everything will work normally
    if (valuesSize > placeholdersCount)
        qWarning().noquote()
                << u"The values.size() > placeholdersCount, the higher number "
                    "of prepared bindings. The current number of placeholder markers is "
                    "'%1' and the number of bound values is '%2', but everything will "
                    "work normally, in %3()."_s
                   .arg(placeholdersCount).arg(valuesSize).arg(__tiny_func__);

    /* This is a problem, prepared bindings processing will be skipped and exec() will
       return an error during the mysql_stmt_execute() call. */
    if (placeholdersCount > valuesSize)
        qWarning().noquote()
                << u"The placeholdersCount > values.size(), an insufficient number "
                    "of prepared bindings. The current number of placeholder markers is "
                    "'%1' and the number of bound values is '%2', skipping prepared "
                    "bindings processing, in %3()."_s
                   .arg(placeholdersCount).arg(valuesSize).arg(__tiny_func__);
}

void MySqlResultPrivate::bindPreparedBindings(
        QList<my_bool> &nullVector, QList<QByteArray> &stringVector,
        QList<MYSQL_TIME> &timeVector)
{
    // Reserve all vectors for prepared bindings buffer data
    reserveVectorsForBindings(nullVector, stringVector, timeVector);
    // Allocate memory for prepared bindings that will be sent to the database
    allocateMemoryForBindings(preparedBinds,
                              static_cast<std::size_t>(boundValues.size()));

    /*! Alias for bound values size type. */
    using BoundValuesSizeType = decltype (boundValues)::size_type;
    for (BoundValuesSizeType index = 0; index < boundValues.size(); ++index) {
        // MySQL storage for prepared binding to prepare
        auto &preparedBind = preparedBinds[static_cast<std::size_t>(index)];
        // Prepared binding value to prepare
        const auto &boundValue = boundValues.at(index);
        // Pointer to a raw data to bind (of course some types need special handling)
        auto *const data = const_cast<void *>(boundValue.constData());

        // Emplace to the vector to make it alive until mysql_stmt_execute()
        preparedBind.is_null = &nullVector.emplaceBack(
                                   static_cast<my_bool>(
                                       SqlResultPrivate::isVariantNull(boundValue)));
        preparedBind.length = nullptr;
        preparedBind.is_unsigned = false;

        switch (boundValue.typeId()) {
        case QMetaType::QByteArray:
            preparedBind.buffer_type   = MYSQL_TYPE_BLOB;
            preparedBind.buffer_length = static_cast<ulong>(
                                             boundValue.toByteArray().size());
            /* The toByteArray().constData() is correct, it will point to the same
               data even if the QVariant creates a copy of the QByteArray inside
               toByteArray(). Also, need to use the constData() to avoid detach. */
            preparedBind.buffer = const_cast<char *>(
                                      boundValue.toByteArray().constData());
            break;

        case QMetaType::QTime:
        case QMetaType::QDate:
        case QMetaType::QDateTime:
            preparedBind.buffer_length = sizeof (MYSQL_TIME);
            // Emplace to the vector to make it alive until mysql_stmt_execute()
            preparedBind.buffer = &timeVector.emplaceBack(
                                      toMySqlDateTime(
                                          boundValue.toDate(), boundValue.toTime(),
                                          boundValue.typeId(), preparedBind));
            break;

        case QMetaType::UInt:
        case QMetaType::Int:
            preparedBind.buffer_type   = MYSQL_TYPE_LONG;
            preparedBind.buffer_length = sizeof (int);
            preparedBind.is_unsigned   = boundValue.typeId() == QMetaType::UInt;
            preparedBind.buffer        = data;
            break;

        case QMetaType::Bool:
            preparedBind.buffer_type   = MYSQL_TYPE_TINY;
            preparedBind.buffer_length = sizeof (bool);
            preparedBind.is_unsigned   = false;
            preparedBind.buffer        = data;
            break;

        case QMetaType::Double:
            preparedBind.buffer_type   = MYSQL_TYPE_DOUBLE;
            preparedBind.buffer_length = sizeof (double);
            preparedBind.buffer        = data;
            break;

        case QMetaType::LongLong:
        case QMetaType::ULongLong:
            preparedBind.buffer_type   = MYSQL_TYPE_LONGLONG;
            preparedBind.buffer_length = sizeof (qint64);
            preparedBind.is_unsigned   = boundValue.typeId() == QMetaType::ULongLong;
            preparedBind.buffer        = data;
            break;

        case QMetaType::QString:
        default: {
            // Emplace to the vector to make it alive until mysql_stmt_execute()
            const auto &stringRef = stringVector.emplaceBack(
                                        boundValue.toString().toUtf8());

            preparedBind.buffer_type   = MYSQL_TYPE_STRING;
            preparedBind.buffer_length = static_cast<ulong>(stringRef.size());
            preparedBind.buffer        = const_cast<char *>(stringRef.constData());
            break;
        }
        }
    }
}

void MySqlResultPrivate::bindResultBlobs()
{
   for (ResultFieldsSizeType index = 0; index < resultFields.size(); ++index) {
       auto &resultBind = resultBinds[index];
       auto &resultField = resultFields[index];
       const auto *const fieldInfo = resultField.myField;

       // Nothing to do, isn't the BLOB type or some info is missing
       if (!isBlobType(resultBind.buffer_type) || meta == nullptr || fieldInfo == nullptr)
           continue;

       // Update the buffer length to the BLOB max. length in the current result set
       resultBind.buffer_length = fieldInfo->max_length;

       /* Create a new BLOB result buffer using a new BLOB length.
          The previous BLOB buffer will be auto-freed as it's a smart pointer. */
       resultField.fieldValue = std::make_unique<char[]>(fieldInfo->max_length); // NOLINT(modernize-avoid-c-arrays)
       resultBind.buffer = static_cast<void *>(resultField.fieldValue.get());
   }
}

/* Result sets */

std::optional<QString>
MySqlResultPrivate::errorMessageForStmtFetch(const int status) noexcept
{
    // Nothing to do, fetching was successful
    if (status == 0)
        return std::nullopt;

    if (status == 1)
        return u"Unable to fetch data in %1()."_s;

    if (status == MYSQL_DATA_TRUNCATED)
        return u"Data truncated during fetching data in %1()."_s;

    Q_UNREACHABLE();
}

QVariant MySqlResultPrivate::getValueForNormal(const ResultFieldsSizeType index) const
{
    const auto &field = resultFields[index]; // Index bounds checked in MySqlResult::data()
    const auto *const column = row[index];

    // Field is NULL
    if (column == nullptr)
        return QVariant(field.metaType);

    // BIT field
    if (isBitType(field.myField->type))
        return toBitField(field, column);

    const auto typeId = field.metaType.id();
    const auto fieldLength = mysql_fetch_lengths(result)[index];

    // BLOB field needs QVariant(QByteArray) as the storage
    if (typeId == QMetaType::QByteArray)
        return QByteArray(row[index], static_cast<QByteArray::size_type>(fieldLength));

    return createQVariant(typeId, QString::fromUtf8(column,
                                                    static_cast<QString::size_type>(
                                                        fieldLength)));
}

QVariant MySqlResultPrivate::getValueForPrepared(const ResultFieldsSizeType index) const
{
    const auto &field = resultFields[index]; // Index bounds checked in MySqlResult::data()

    // Field is NULL
    if (field.isNull)
        return QVariant(field.metaType);

    // BIT field
    if (isBitType(field.myField->type))
        return toBitField(field, field.fieldValue.get());

    const auto typeId = field.metaType.id();

    /* Create an integer QVariant by the given metatype ID and value. This early integer
       number if() condition still makes sense (even if the same logic is already
       inside the createQVariant() method) because it skips QString::fromUtf8()
       conversion and instead constructs the QVariant() directly from the field value. */
    if (MySqlUtils::isInteger(typeId))
        return createIntegerQVariant(typeId, field);

    /* Here is different logic as for the normal queries because normal queries return
       datetime-related columns as a char * but prepared statements return MYSQL_TIME. */
    if (MySqlUtils::isTimeOrDate(field.myField->type) &&
        field.fieldValueSize >= sizeof (MYSQL_TIME)
    )
        return toQDateTimeFromMySQLTime(typeId, reinterpret_cast<const MYSQL_TIME *>(
                                                    field.fieldValue.get()));

    // The BLOB field needs QVariant(QByteArray) as storage
    if (typeId == QMetaType::QByteArray)
        return toQByteArray(index);

    // Create a QVariant by the given metatype ID and field value
    return createQVariant(typeId, QString::fromUtf8(field.fieldValue.get(),
                                                    static_cast<QString::size_type>(
                                                        field.fieldValueSize)));
}

void
MySqlResultPrivate::throwIfBadResultFieldsIndex(const ResultFieldsSizeType index) const
{
    const auto fieldsCount = resultFields.size();

    // Nothing to do
    if (index >= 0 || index < fieldsCount)
        return;

    throw Exceptions::OutOfRangeError(
                u"The field index '%1' is out of bounds, the index must be between 0-%2 "
                 "in %3()."_s
                .arg(index).arg(fieldsCount - 1).arg(__tiny_func__));
}

/* private */

/* Prepared queries */

bool MySqlResultPrivate::wasAllFieldsFetched(
        const uint fieldsCount, const uint lastIndex, const QLatin1StringView method)
{
    /* This verification comparison is weird but is correct, the index will be increased
       after the last column fetch so the index will match the count. 🤯 */
    if (const auto allFieldsFetched = fieldsCount == lastIndex; allFieldsFetched)
        return true;

    /* Also, the warning message does not match the method name at 100%, but it means
       that the last successfully fetched column has the 'lastIndex', so not all fields
       have been fetched. */
    qWarning().noquote()
            << u"The fieldsCount != index, column with the index '%1' has no metadata, "
                "so not all fields have been fetched, this should never happen :/, "
                "in MySqlResultPrivate::%2()."_s
               .arg(lastIndex).arg(method);

    return false;
}

/* All these modernize-avoid-c-arrays suppressions are correct as we only need to allocate
   buffers on the heap, nothing else, we don't need access, modify, iterate over, or
   anything else, just simple buffers and C arrays are perfect for this.
   The std::array can't be used here as we don't know the size and std::vector is too
   much. */

void MySqlResultPrivate::allocateMemoryForBindings(std::unique_ptr<MYSQL_BIND[]> &binds, // NOLINT(modernize-avoid-c-arrays)
                                                   const std::size_t count) noexcept
{
    binds = std::make_unique<MYSQL_BIND[]>(count); // NOLINT(modernize-avoid-c-arrays)
    // Zero the memory storage
    memset(binds.get(), 0, sizeof (MYSQL_BIND) * count);
}

void MySqlResultPrivate::reserveVectorsForBindings(
        QList<my_bool> &nullVector, QList<QByteArray> &stringVector,
        QList<MYSQL_TIME> &timeVector) const
{
    nullVector.reserve(boundValues.size());

    stringVector.reserve(std::ranges::count_if(boundValues, [](const QVariant &value)
    {
        return value.typeId() == QMetaType::QString;
    }));

    timeVector.reserve(std::ranges::count_if(boundValues, [](const QVariant &value)
    {
        const auto typeId = value.typeId();
        /* All QMetaType-s that will be converted to the MYSQL_TIME, must exactly match
           the case statements in the bindPreparedBindings(). */
        return typeId == QMetaType::QDateTime ||
               typeId == QMetaType::QDate     ||
               typeId == QMetaType::QTime;
    }));
}

bool MySqlResultPrivate::isBlobType(const enum_field_types fieldType) noexcept
{
    return fieldType == MYSQL_TYPE_BLOB        ||
           fieldType == MYSQL_TYPE_MEDIUM_BLOB ||
           fieldType == MYSQL_TYPE_TINY_BLOB   ||
// MYSQL_TYPE_JSON was added in MySQL 5.7.8
#if defined(MYSQL_VERSION_ID) && MYSQL_VERSION_ID >= 50708
           fieldType == MYSQL_TYPE_JSON        ||
#endif
           fieldType == MYSQL_TYPE_LONG_BLOB;
}

MYSQL_TIME
MySqlResultPrivate::toMySqlDateTime(const QDate date, const QTime time, const int typeId,
                                    MYSQL_BIND &bind)
{
    // This method is used for prepared bindings

    Q_ASSERT(typeId == QMetaType::QDateTime || typeId == QMetaType::QTime ||
             typeId == QMetaType::QDate);

    // Create and zero the MYSQL_TIME structure as 0000-00-00 00:00:00.000000
    auto mysqlTime = createMsqlTime();

    const auto initDate = [&mysqlTime, date]
    {
        mysqlTime.year  = static_cast<uint>(date.year());
        mysqlTime.month = static_cast<uint>(date.month());
        mysqlTime.day   = static_cast<uint>(date.day());
    };

    const auto initTime = [&mysqlTime, time]
    {
        mysqlTime.hour        = static_cast<uint>(time.hour());
        mysqlTime.minute      = static_cast<uint>(time.minute());
        mysqlTime.second      = static_cast<uint>(time.second());
        mysqlTime.second_part = static_cast<ulong>(time.msec()) * 1000; // Convert from milliseconds (ms) to microseconds
    };

    /* We don't need to set the MYSQL_TIME.time_type as they are not during preparing
       statements. */

    switch (typeId) {
    case QMetaType::QDateTime:
        initDate();
        initTime();

        // mysqlTime.time_type = MYSQL_TIMESTAMP_DATETIME;
        bind.buffer_type = MYSQL_TYPE_DATETIME;
        break;

    case QMetaType::QDate:
        initDate();

        // mysqlTime.time_type = MYSQL_TIMESTAMP_DATE;
        bind.buffer_type = MYSQL_TYPE_DATE;
        break;

    case QMetaType::QTime:
        initTime();

        // mysqlTime.time_type = MYSQL_TIMESTAMP_TIME;
        bind.buffer_type = MYSQL_TYPE_TIME;
        break;

    default:
        Q_UNREACHABLE();
    }

    return mysqlTime;
}

MYSQL_TIME MySqlResultPrivate::createMsqlTime() noexcept
{
    MYSQL_TIME mysqlTime;
    // It causes bugs eg. -838:59:59 time if isn't zero-ed
    memset(&mysqlTime, 0, sizeof (MYSQL_TIME));

    return mysqlTime;
}

/* Result sets */

quint64 MySqlResultPrivate::toBitField(const MyField &field,
                                       const char *const fieldValue) noexcept
{
    // Nothing to do
    if (fieldValue == nullptr)
        return 0;

    /* +7 is pure math, hard to explain how this works, try to compute for 1 bit to 9 bit
       and it will immediately be clear how this works. Also, I have revisited whole
       method and everything is ok. */

    // Byte-aligned length
    const auto numBytes = (field.myField->length + 7) / 8;
    quint64 result = 0;

    for (ulong index = 0; index < numBytes; ++index) {
        const quint64 tmp = static_cast<quint8>(fieldValue[index]);
        /* Shift the current value 8 bit-s left to make 8 bit space (zero-ed)
           for the next 8 bit chunk (tmp quint8!). */
        result <<= 8;
        // Then OR this new 8 bit-s from the tmp to this prepared empty 8-bit space
        result |= tmp;
    }

    return result;
}

QVariant MySqlResultPrivate::toQDateFromString(const QString &value)
{
    // This method is used for normal queries only

    /* This should never happen :/, MySQL always returns date/time even if
       the NO_ZERO_DATE option isn't defined.*/
    Q_ASSERT(!value.isEmpty());

    // Nothing to do
    if (value.isEmpty())
        return QDate();

    return QDate::fromString(value, Qt::ISODate);
}

// This method is never called as the MYSQL_TYPE_TIME is mapped to the QString
// QVariant MySqlResultPrivate::toQTimeFromString(const QString &value)
// {
//    /* This should never happen :/, MySQL always returns date/time even if
//       the NO_ZERO_DATE option isn't defined.*/
//     Q_ASSERT(!value.isEmpty());

//     // Nothing to do
//     if (value.isEmpty())
//         return QTime();

//     return QTime::fromString(value, Qt::ISODateWithMs);
// }

QVariant MySqlResultPrivate::toQDateTimeFromString(QString value)
{
    // This method is used for normal queries only

    /* This should never happen :/, MySQL always returns date/time even if
       the NO_ZERO_DATE option isn't defined.*/
    Q_ASSERT(!value.isEmpty());

    // Nothing to do
    if (value.isEmpty())
        return QDateTime();

    /* Some MySQL functions can return datetime/timestamp-s in the YYYYMMDDhhmmss format,
       depending on whether the function is used in string or numeric context,
       eg. NOW() + 0 will return in this format, so this if condition makes sense. */
    if (value.size() == 14 && MySqlUtils::isNumber(value))
        value.insert(4, DASH).insert(7, DASH).insert(10, 'T'_L1).insert(13, COLON)
             .insert(16, COLON);

    return QDateTime::fromString(value, Qt::ISODateWithMs);
}

QVariant
MySqlResultPrivate::toQDateTimeFromMySQLTime(const int typeId,
                                             const MYSQL_TIME *const mysqlTime)
{
    QDate date;
    QTime time;

    if (typeId != QMetaType::QTime)
        date = {static_cast<int>(mysqlTime->year),
                static_cast<int>(mysqlTime->month),
                static_cast<int>(mysqlTime->day)};

    if (typeId != QMetaType::QDate)
        time = {static_cast<int>(mysqlTime->hour),
                static_cast<int>(mysqlTime->minute),
                static_cast<int>(mysqlTime->second),
                static_cast<int>(
                    std::lround(
                        static_cast<double>(mysqlTime->second_part) / 1000))};

    if (typeId == QMetaType::QDateTime)
        return QDateTime(date, time);

    if (typeId == QMetaType::QDate)
        return date;

    /* It never can have the QMetaType::QTime typeId because we are mapping
       the MYSQL_TYPE_TIME to QMetaType::QString in the decodeMySqlType() method above. */
    // if (typeId == QMetaType::QTime)
    //     return time;

    Q_UNREACHABLE();
}

QVariant MySqlResultPrivate::toDoubleFromString(const QString &value) const
{
    auto ok = false;
    const double valueDouble = value.toDouble(&ok);

    if (!ok) {
        qWarning().noquote()
            << u"Unable to convert QString to the double type, QString value is '%1'. "
                "Returning invalid QVariant(), in %2()."_s.arg(value, __tiny_func__);
        return {};
    }

    /*! Expose the NumericalPrecisionPolicy enum. */
    using enum Orm::Drivers::NumericalPrecisionPolicy;

    switch(precisionPolicy) {
    // This is the default precision policy
    case LowPrecisionDouble:
        return valueDouble;

    // The following two precision policies discard the fractional part silently
    case LowPrecisionInt32:
        return QVariant(valueDouble).toInt();
    case LowPrecisionInt64:
        return QVariant(valueDouble).toLongLong();

    case HighPrecision:
        return value;

    default:
        Q_UNREACHABLE();
    }
}

QVariant MySqlResultPrivate::toQByteArray(const ResultFieldsSizeType index) const
{
    // This method is called for prepared queries only

    // Index bounds checked in MySqlResult::data()
    const auto &field = resultFields[index];

    return {QByteArray(field.fieldValue.get(),
                       static_cast<QByteArray::size_type>(field.fieldValueSize))};
}

QVariant
MySqlResultPrivate::createQVariant(const int typeId, QString &&value) const
{
    switch (typeId) {
    /* Also, the MYSQL_TYPE_TIME is returned as the QString (normal/prepared) because of
       '-838:59:59' to '838:59:59' range. */
    case QMetaType::QString:
        return value;

    case QMetaType::LongLong:
        return value.toLongLong();

    case QMetaType::ULongLong:
        return value.toULongLong();

    case QMetaType::Char:
    case QMetaType::Short:
    case QMetaType::Int:
        return value.toInt();

    case QMetaType::UChar:
    case QMetaType::UShort:
    case QMetaType::UInt:
        return value.toUInt();

    /* For normal queries only, prepared queries have separate logic for QDateTime and
       QDate because date/time column types for prepared queries are returned using
       the MYSQL_TIME structure. */
    case QMetaType::QDateTime:
        Q_ASSERT(!preparedQuery);
        return toQDateTimeFromString(std::move(value));

    case QMetaType::QDate: // The same as for the QMetaType::QDateTime is true here
        Q_ASSERT(!preparedQuery);
        return toQDateFromString(value);

    case QMetaType::Double:
        return toDoubleFromString(value);

    /* The QMetaType::QTime is never reached because the MYSQL_TYPE_TIME is mapped
       to the QString. So moved down as the last case statement. QByteArray is always
       handled earlier. */
    case QMetaType::QByteArray:
    case QMetaType::QTime:
        Q_UNREACHABLE();

    default:
        return value;
    }

    Q_UNREACHABLE();
}

QVariant
MySqlResultPrivate::createIntegerQVariant(const int typeId, const MyField &field)
{
    QVariant integer(field.metaType, field.fieldValue.get());

    // Avoid QVariant(char) for TINYINT to prevent weird conversions (QTBUG-53397)
    if (typeId == QMetaType::UChar)
        return integer.toUInt();

    if (typeId == QMetaType::Char)
        return integer.toInt();

    return integer;
}

} // namespace Orm::Drivers::MySql

TINYORM_END_COMMON_NAMESPACE
