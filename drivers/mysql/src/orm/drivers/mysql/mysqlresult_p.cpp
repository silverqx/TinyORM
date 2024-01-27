#include "orm/drivers/mysql/mysqlresult_p.hpp"

#if QT_CONFIG(datestring)
#  include <QDateTime>

#  include "orm/drivers/mysql/mysqlconstants_p.hpp"
#endif

#include "orm/drivers/mysql/mysqlutils_p.hpp"
#include "orm/drivers/utils/helpers_p.hpp"

// CUR drivers extract common MySQL types to own file, then also check throwIfBadResultFieldsIndex(std::size_t) vs ResultFieldsType silverqx
// this is a copy of the old MYSQL_TIME before an additional integer was added in
// 8.0.27.0. This kills the sanity check during retrieving this struct from mysql
// when another libmysql version is used during runtime than during compile time
struct QT_MYSQL_TIME
{
    unsigned int year = 0;
    unsigned int month = 0;
    unsigned int day = 0;
    unsigned int hour = 0;
    unsigned int minute = 0;
    unsigned int second = 0;

    /*! The fractional part of the second in microseconds. */
    unsigned long second_part = 0; // NOLINT(google-runtime-int)
    my_bool neg = false;
    enum enum_mysql_timestamp_type time_type = MYSQL_TIMESTAMP_NONE;
};

TINYORM_BEGIN_COMMON_NAMESPACE

using namespace Qt::StringLiterals; // NOLINT(google-build-using-namespace)

using Orm::Drivers::MySql::Constants::COLON;
using Orm::Drivers::MySql::Constants::DASH;

using Orm::Drivers::Utils::Helpers;

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

    // CUR drivers test with no metadata silverqx
    // Nothing to do, metadata disabled
    if (mysql_result_metadata(result) == RESULTSET_METADATA_NONE) {
        qWarning("MySqlResultPrivate::populateFields: result set has no metadata");
        return false;
    }

    uint index = 0;
    const MYSQL_FIELD *fieldInfo = nullptr;
    resultFields.resize(fieldsCount);

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
    // Obtain result set metadata
    if (meta = mysql_stmt_result_metadata(stmt); meta == nullptr)
        /* Don't log a warning about no metadata here as the INSERT, UPDATE, and DELETE
           queries have no metadata. */
        return false;

    const auto fieldsCount = mysql_num_fields(meta);
    const auto hasFields = fieldsCount > 0;

    // Nothing to populate, no result set
    if (!hasFields)
        return false;

    // CUR drivers for prepared statements it has RESULTSET_METADATA_NONE (0), need to try disable metadata and examine the changes in the meta silverqx
    // Nothing to do, metadata disabled
//    if (mysql_result_metadata(meta) == RESULTSET_METADATA_NONE) {
//        qWarning("MySqlResultPrivate::bindResultValues: result set has no metadata");
//        return false;
//    }

    // Allocate memory for result sets that will be obtained from the database
    allocateMemoryForBindings(resultBinds, fieldsCount);

    uint index = 0;
    const MYSQL_FIELD *fieldInfo = nullptr;
    resultFields.resize(fieldsCount);

    while((fieldInfo = mysql_fetch_field(meta)) != nullptr) {
        auto &resultBind = resultBinds[index];
        auto &field = resultFields[index];

        field.myField = fieldInfo;
        field.metaType = MySqlUtils::decodeMySqlType(fieldInfo->type, fieldInfo->flags);
        resultBind.buffer_type = fieldInfo->type;

        // CUR drivers finish this during testing BLOB-s silverqx
        if (isBlobType(fieldInfo->type)) {
            /* The size of a blob-field is available as soon as
               the mysql_stmt_store_result() is called, it's after
               the mysql_stmt_exec() in MySqlResult::exec(). */
            resultBind.buffer_length = field.fieldValueSize = 0UL;
            hasBlobs = true;
        }
        else if (MySqlUtils::isTimeOrDate(fieldInfo->type))
            resultBind.buffer_length = field.fieldValueSize = sizeof (QT_MYSQL_TIME);

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

        /* Prepare the output/result buffer (nothing to do with prepared bindings),
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
                << u"MySqlResultPrivate::hasPreparedBindings: "
                    "values.size() > placeholdersCount, "
                    "higher number of prepared bindings; Current number of "
                    "placeholder markers is '%1' and number of bind values is '%2', "
                    "but everything will work normally"_s
                   .arg(placeholdersCount)
                   .arg(valuesSize);

    /* This is a problem, prepared bindings processing will be skipped and exec() will
       return an error. */
    if (placeholdersCount > valuesSize)
        qWarning().noquote()
                << u"MySqlResultPrivate::hasPreparedBindings: "
                    "placeholdersCount > values.size(), "
                    "insufficient number of prepared bindings; Current number of "
                    "placeholder markers is '%1' and number of bind values is '%2', "
                    "skipping prepared bindings processing"_s
                   .arg(placeholdersCount)
                   .arg(valuesSize);
}

void MySqlResultPrivate::bindPreparedBindings(
        QList<my_bool> &nullVector, QList<QByteArray> &stringVector,
        QList<QT_MYSQL_TIME> &timeVector)
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

        switch (boundValue.userType()) {
        case QMetaType::QByteArray:
            preparedBind.buffer_type   = MYSQL_TYPE_BLOB;
            // Need to use the constData() to avoid detach
            preparedBind.buffer_length = static_cast<ulong>(
                                             boundValue.toByteArray().size());
            /* The toByteArray().constData() is correct, it will point to the same
               data even if the QVariant creates a copy of the QByteArray inside
               toByteArray(). */
            preparedBind.buffer = const_cast<char *>(
                                      boundValue.toByteArray().constData());
            break;

        case QMetaType::QTime:
        case QMetaType::QDate:
        case QMetaType::QDateTime:
            preparedBind.buffer_length = sizeof (QT_MYSQL_TIME);
            // Emplace to the vector to make it alive until mysql_stmt_execute()
            preparedBind.buffer = &timeVector.emplaceBack(
                                      toMySqlDateTime(
                                          boundValue.toDate(), boundValue.toTime(),
                                          boundValue.userType(), preparedBind));
            break;

        case QMetaType::UInt:
        case QMetaType::Int:
            preparedBind.buffer_type   = MYSQL_TYPE_LONG;
            preparedBind.buffer_length = sizeof (int);
            preparedBind.is_unsigned   = boundValue.userType() != QMetaType::Int;
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
            preparedBind.is_unsigned   = boundValue.userType() == QMetaType::ULongLong;
            preparedBind.buffer        = data;
            break;

        case QMetaType::QString:
        // CUR drivers finish, test if is convertible to QString and do something with it silverqx
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

SqlError
MySqlResultPrivate::createStmtError(const QString &error, const SqlError::ErrorType type,
                                    MYSQL_STMT *const stmt)
{
    Q_ASSERT(stmt != nullptr);

    const auto *const mysqlError = mysql_stmt_error(stmt);

    return {"QMYSQL: "_L1 + error, QString::fromUtf8(mysqlError), type,
            QString::number(mysql_stmt_errno(stmt))};
}

/* Result sets */

std::optional<QString> MySqlResultPrivate::fetchErrorMessage(const int status) noexcept
{
    // Nothing to do, fetching was successful
    if (status == 0)
        return std::nullopt;

    if (status == 1)
        return u"Unable to fetch data"_s;

    if (status == MYSQL_DATA_TRUNCATED)
        return u"Data truncated during fetching data"_s;

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
        return QVariant::fromValue(toBitField(field, column));

    QString value;
    std::size_t fieldLength = 0;
    const auto typeId = field.metaType.id();
    // CUR drivers revisit silverqx
    fieldLength = mysql_fetch_lengths(result)[index];

    // BLOB field needs the QByteArray as the storage
    if (typeId != QMetaType::QByteArray)
        value = QString::fromUtf8(column, static_cast<QString::size_type>(fieldLength));

    return createQVariant(typeId, std::move(value), index);
}

QVariant MySqlResultPrivate::getValueForPrepared(const ResultFieldsSizeType index) const
{
    const auto &field = resultFields[index]; // Index bounds checked in MySqlResult::data()

    // Field is NULL
    if (field.isNull)
        return QVariant(field.metaType);

    // BIT field
    if (isBitType(field.myField->type))
        return QVariant::fromValue(toBitField(field, field.fieldValue.get()));

    const auto typeId = field.metaType.id();

    // CUR drivers the following isInteger() block make prepared statements behavior different from normal queries, all logic is handled in the createQVariant() at the end of this method, I don't understand why here is different logic for prepared statements silverqx
    // CUR drivers finish silverqx
    // BUG drivers I think this should be here, it's correctly handled in the createQVariant() silverqx
    if (MySqlUtilsPrivate::isInteger(typeId)) {
        QVariant integer(field.metaType, field.fieldValue.get());

        // Avoid QVariant(char) for TINYINT to prevent weird conversions (QTBUG-53397)
        if (typeId == QMetaType::UChar)
            return integer.toUInt();

        if (typeId == QMetaType::Char)
            return integer.toInt();

        return integer;
    }

    /* Here is different logic as for the normal queries because normal queries return
       datetime-related columns as a char * but prepared statements return MYSQL_TIME. */
    if (MySqlUtils::isTimeOrDate(field.myField->type) &&
        field.fieldValueSize >= sizeof (QT_MYSQL_TIME)
    )
        return toQDateTimeFromMySQLTime(typeId, reinterpret_cast<const QT_MYSQL_TIME *>(
                                                    field.fieldValue.get()));

    QString value;

    // The BLOB field needs the QByteArray as storage
    if (typeId != QMetaType::QByteArray)
        value = QString::fromUtf8(field.fieldValue.get(), field.fieldValueSize);

    return createQVariant(typeId, std::move(value), index);
}

/* private */

/* Prepared queries */

bool MySqlResultPrivate::wasAllFieldsFetched(
        const uint fieldsCount, const uint lastIndex, const QLatin1StringView method)
{
    /* This verification comparison is weird but is correct, the index will be increased
       after the last fetch so the index will match the count. 🤯 */
    if (const auto allFieldsFetched = fieldsCount == lastIndex; allFieldsFetched)
        return true;

    qWarning().noquote()
            << u"MySqlResultPrivate::%1: fieldsCount != index, column "
                "with the index '%2' has no metadata, this should never happen :/"_s
               .arg(method).arg(lastIndex);

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
        QList<QT_MYSQL_TIME> &timeVector) const
{
    nullVector.reserve(boundValues.size());

    stringVector.reserve(std::ranges::count_if(boundValues, [](const QVariant &value)
    {
        return Helpers::qVariantTypeId(value) == QMetaType::QString;
    }));

    timeVector.reserve(std::ranges::count_if(boundValues, [](const QVariant &value)
    {
        const auto typeId = Helpers::qVariantTypeId(value);

        return typeId == QMetaType::QDateTime ||
               typeId == QMetaType::QDate     ||
               typeId == QMetaType::QTime;
    }));
}

bool MySqlResultPrivate::isBlobType(const enum_field_types fieldType) noexcept
{
    return fieldType == MYSQL_TYPE_TINY_BLOB   ||
           fieldType == MYSQL_TYPE_BLOB        ||
           fieldType == MYSQL_TYPE_MEDIUM_BLOB ||
           fieldType == MYSQL_TYPE_LONG_BLOB   ||
           fieldType == MYSQL_TYPE_JSON;
}

QT_MYSQL_TIME
MySqlResultPrivate::toMySqlDateTime(const QDate date, const QTime time, const int typeId,
                                    MYSQL_BIND &bind)
{
    Q_ASSERT(typeId == QMetaType::QDateTime || typeId == QMetaType::QTime ||
             typeId == QMetaType::QDate);

    QT_MYSQL_TIME mysqlTime;

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
        mysqlTime.second_part = static_cast<ulong>(time.msec()) * 1000;
    };

    switch (typeId) {
    case QMetaType::QDateTime:
        initDate();
        initTime();

        mysqlTime.time_type = MYSQL_TIMESTAMP_DATETIME;
        bind.buffer_type = MYSQL_TYPE_DATETIME;
        break;

    case QMetaType::QDate:
        initDate();

        mysqlTime.time_type = MYSQL_TIMESTAMP_DATE;
        bind.buffer_type = MYSQL_TYPE_DATE;
        break;

    case QMetaType::QTime:
        initTime();

        mysqlTime.time_type = MYSQL_TIMESTAMP_TIME;
        bind.buffer_type = MYSQL_TYPE_TIME;
        break;

    default:
        Q_UNREACHABLE();
    }

    return mysqlTime;
}

/* Result sets */

quint64 MySqlResultPrivate::toBitField(const MyField &field,
                                       const char *const fieldValue) noexcept
{
    // Nothing to do
    if (fieldValue == nullptr)
        return 0;

    // CUR drivers find out why +7 and add the comment silverqx
    // Byte-aligned length
    const auto numBytes = (field.myField->length + 7) / 8;
    quint64 result = 0;

    for (ulong index = 0; index < numBytes; ++index) {
        const quint64 tmp = static_cast<quint8>(fieldValue[index]);
        result <<= 8;
        result |= tmp;
    }

    return result;
}

QVariant MySqlResultPrivate::toQDateFromString(const QString &value)
{
#if QT_CONFIG(datestring)
    // Nothing to do
    if (value.isEmpty())
        // CUR drivers check this if QVariant(QMetaType(QMetaType::QDate)) is different thing; also Qt5 vs Qt6; also for QTime() and QDateTime() below silverqx
        return QDate();

    return QDate::fromString(value, Qt::ISODate);
#else
    Q_UNUSED(value);
    return QVariant(value);
#endif
}

QVariant MySqlResultPrivate::toQTimeFromString(const QString &value)
{
#if QT_CONFIG(datestring)
    // Nothing to do
    if (value.isEmpty())
        return QTime();

    return QTime::fromString(value, Qt::ISODate);
#else
    Q_UNUSED(value);
    return QVariant(value);
#endif
}

QVariant MySqlResultPrivate::toQDateTimeFromString(QString value)
{
#if QT_CONFIG(datestring)
    // Nothing to do
    if (value.isEmpty())
        return QDateTime();

    // CUR drivers revisit and if it's true the also validate 14 digits string silverqx
    // TIMESTAMPS have the format yyyyMMddhhmmss
    if (value.size() == 14)
        value.insert(4, DASH).insert(7, DASH).insert(10, 'T'_L1).insert(13, COLON)
             .insert(16, COLON);

    return QDateTime::fromString(value, Qt::ISODate);
#else
    Q_UNUSED(value);
    return QVariant(value);
#endif
}

QVariant
MySqlResultPrivate::toQDateTimeFromMySQLTime(const int typeId,
                                             const QT_MYSQL_TIME *const mysqlTime)
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

    if (typeId == QMetaType::QTime)
        return time;

    Q_UNREACHABLE();
}

QVariant MySqlResultPrivate::toDoubleFromString(const QString &value) const
{
    // CUR drivers use String::isNumber()? silverqx
    auto ok = false;
    // CUR drivers test this with the HighPrecision policy silverqx
    const double valueDouble = value.toDouble(&ok);

    if (!ok) {
        qWarning().noquote()
            << u"MySqlResultPrivate::toQVariantDouble: unable to convert QString "
                "to the double type, QString value is '%1'"_s.arg(value);
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

QVariant
MySqlResultPrivate::toQByteArray(const ResultFieldsSizeType index) const
{
    if (preparedQuery) {
        const auto &field = resultFields[index]; // Index bounds checked in MySqlResult::data()
        return {QByteArray(field.fieldValue.get(),
                           static_cast<QByteArray::size_type>(field.fieldValueSize))};
    }

    // CUR drivers revisit this if I need to fetch the length silverqx
    const auto fieldLength = mysql_fetch_lengths(result)[index];
    return {QByteArray(row[index], static_cast<QByteArray::size_type>(fieldLength))};
}

QVariant MySqlResultPrivate::createQVariant(const int typeId, QString &&value,
                                            const ResultFieldsSizeType index) const
{
    switch (typeId) {
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

    case QMetaType::QDateTime:
        return toQDateTimeFromString(std::move(value));

    case QMetaType::QDate:
        return toQDateFromString(value);

    case QMetaType::QTime:
        return toQTimeFromString(value);

    case QMetaType::Double:
        return toDoubleFromString(value);

    case QMetaType::QByteArray:
        return toQByteArray(index);

    default:
        return value;
    }

    Q_UNREACHABLE();
}

} // namespace Orm::Drivers::MySql

TINYORM_END_COMMON_NAMESPACE
