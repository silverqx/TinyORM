#include "orm/drivers/mysql/mysqlresult_p.hpp"

#if QT_CONFIG(datestring)
#  include <QDateTime>
#endif

#include <orm/utils/helpers.hpp>

#include "orm/drivers/mysql/mysqlutils_p.hpp"

// this is a copy of the old MYSQL_TIME before an additional integer was added in
// 8.0.27.0. This kills the sanity check during retrieving this struct from mysql
// when another libmysql version is used during runtime than during compile time
struct QT_MYSQL_TIME
{
    unsigned int year, month, day, hour, minute, second = 0;
    /*! The fractional part of the second in microseconds. */
    unsigned long second_part = 0;
    my_bool neg = false;
    enum enum_mysql_timestamp_type time_type = MYSQL_TIMESTAMP_NONE;
};

TINYORM_BEGIN_COMMON_NAMESPACE

using Helpers = Orm::Utils::Helpers;

using MySqlUtils = Orm::Drivers::MySql::MySqlUtilsPrivate;

using namespace Qt::StringLiterals;

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
    fields.resize(fieldsCount);

    while((fieldInfo = mysql_fetch_field(result)) != nullptr) {
        fields[index].type = MySqlUtils::decodeMySqlType(fieldInfo->type,
                                                         fieldInfo->flags);
        fields[index].myField = fieldInfo;

        ++index;
    }

    // Executed query has result set
    return hasFields && wasAllFieldsFetched(fieldsCount, index, "populateFields"_L1);
}

bool MySqlResultPrivate::bindInValues()
{
    // Obtain result set metadata
    meta = mysql_stmt_result_metadata(stmt);
    if (meta == nullptr) {
        qWarning("MySqlResultPrivate::bindInValues: result set has no metadata");
        return false;
    }

    const auto fieldsCount = mysql_num_fields(meta);
    const auto hasFields = fieldsCount > 0;

    // Nothing to populate, no result set
    if (!hasFields)
        return false;

    // CUR drivers for prepared stmts it has RESULTSET_METADATA_NONE (0), need to try disable metadata and examine the changes in the meta silverqx
    // Nothing to do, metadata disabled
//    if (mysql_result_metadata(meta) == RESULTSET_METADATA_NONE) {
//        qWarning("MySqlResultPrivate::bindInValues: result set has no metadata");
//        return false;
//    }

    // Allocate memory for result sets that will be obtained from the database
    allocateMemoryForBindings(&inBinds, static_cast<std::size_t>(fieldsCount));

    uint index = 0;
    const MYSQL_FIELD *fieldInfo = nullptr;
    fields.resize(fieldsCount);

    while((fieldInfo = mysql_fetch_field(meta)) != nullptr) {
        MYSQL_BIND *bind = &inBinds[index];
        auto &field = fields[index];

        field.myField = fieldInfo;
        field.type = MySqlUtils::decodeMySqlType(fieldInfo->type, fieldInfo->flags);
        bind->buffer_type = fieldInfo->type;
        // CUR drivers I didn't see +1 for NULL char anywhere on any MySQL docs page in any example, try it with full buffers for more type like char[40] or varchar[255] and find out how it works and behaves silverqx
        // +1 for the NULL character
        bind->buffer_length = field.bufferLength = fieldInfo->length + 1;

        // CUR drivers finish this during testing BLOB-s silverqx
        if (isBlobType(fieldInfo->type)) {
            /* The size of a blob-field is available as soon as
               the mysql_stmt_store_result() is called, it's after
               the mysql_stmt_exec() in MySqlResult::exec(). */
            bind->buffer_length = field.bufferLength = 0;
            hasBlobs = true;
        } else if (MySqlUtils::isTimeOrDate(fieldInfo->type)) {
            bind->buffer_length = field.bufferLength = sizeof (QT_MYSQL_TIME);
        } else if (MySqlUtils::isInteger(field.type.id())) {
            bind->buffer_length = field.bufferLength = 8;
        } else {
            bind->buffer_type = MYSQL_TYPE_STRING;
        }

        // The following two lines only bind data members using pointers (no real values)
        bind->is_null = &field.nullIndicator;
        bind->length  = &field.bufferLength;
        bind->is_unsigned = fieldInfo->flags & UNSIGNED_FLAG;

        /* Prepare the output/result buffer (nothing to do with outBinds or prepared
           bindings), +1 for terminating null character. */
        char *inBuffer = bind->buffer_length > 0 ? new char[bind->buffer_length + 1]
                                                 : nullptr;
        bind->buffer = field.outField = inBuffer;

        ++index;
    }

    // Executed query has result set
    return hasFields && wasAllFieldsFetched(fieldsCount, index, "bindInValues"_L1);
}

bool MySqlResultPrivate::hasPreparedBindings(const ulong placeholdersCount) const
{
    const auto valuesSize = static_cast<ulong>(values.size());

    // This is more likely an info message, everything will work normally
    if (valuesSize > placeholdersCount)
        qWarning().noquote()
                << QStringLiteral(
                       "MySqlResultPrivate::hasPreparedBindings: "
                       "values.size() > placeholdersCount, "
                       "higher number of prepared bindings; Current number of "
                       "placeholder markers is '%1' and number of bind values is '%2', "
                       "but everything will work normally")
                   .arg(placeholdersCount)
                   .arg(values.size());

    if (placeholdersCount > 0 && placeholdersCount <= valuesSize)
        return true;

    qWarning().noquote()
            << QStringLiteral(
                   "MySqlResultPrivate::hasPreparedBindings: "
                   "placeholdersCount > values.size(), "
                   "insufficient number of prepared bindings; Current number of "
                   "placeholder markers is '%1' and number of bind values is '%2'")
               .arg(placeholdersCount)
               .arg(values.size());

    return false;
}

void MySqlResultPrivate::bindPreparedBindings(
        QList<my_bool> &nullVector, QList<QByteArray> &stringVector,
        QList<QT_MYSQL_TIME> &timeVector)
{
    // Reserve all vectors for prepared bindings buffer data
    reserveVectorsForBindings(nullVector, stringVector, timeVector);
    // Allocate memory for prepared bindings that will be sent to the database
    allocateMemoryForBindings(&outBinds, static_cast<std::size_t>(values.size()));

    for (qsizetype index = 0; index < values.size(); ++index) {
        // MySQL storage for prepared binding to prepare
        MYSQL_BIND *outBind = &outBinds[index];
        // Prepared binding value to prepare
        const auto &value = values.at(index);
        // Pointer to a raw data to bind (of course some types need special handling)
        auto *data = const_cast<void *>(value.constData());

        // Emplace to the vector to make it alive until mysql_stmt_execute()
        outBind->is_null = &nullVector.emplaceBack(
                               static_cast<my_bool>(
                                   SqlResultPrivate::isVariantNull(value)));
        outBind->length = 0;
        outBind->is_unsigned = false;

        switch (value.userType()) {
        case QMetaType::QByteArray:
            outBind->buffer_type   = MYSQL_TYPE_BLOB;
            // Need to use the constData() to avoid detach
            outBind->buffer_length = value.toByteArray().size();
            /* The toByteArray().constData() is correct, it will point to the same
               data even if the QVariant creates a copy of the QByteArray inside
               toByteArray(). */
            outBind->buffer = const_cast<char *>(value.toByteArray().constData());
            break;

        case QMetaType::QTime:
        case QMetaType::QDate:
        case QMetaType::QDateTime:
            outBind->buffer_length = sizeof (QT_MYSQL_TIME);
            outBind->length        = 0;
            // Emplace to the vector to make it alive until mysql_stmt_execute()
            outBind->buffer        = &timeVector.emplaceBack(
                                         toMySqlDate(value.toDate(),   value.toTime(),
                                                     value.userType(), outBind));
            break;

        case QMetaType::UInt:
        case QMetaType::Int:
            outBind->buffer_type   = MYSQL_TYPE_LONG;
            outBind->buffer_length = sizeof (int);
            outBind->is_unsigned   = value.userType() != QMetaType::Int;
            outBind->buffer        = data;
            break;

        case QMetaType::Bool:
            outBind->buffer_type   = MYSQL_TYPE_TINY;
            outBind->buffer_length = sizeof (bool);
            outBind->is_unsigned   = false;
            outBind->buffer        = data;
            break;

        case QMetaType::Double:
            outBind->buffer_type   = MYSQL_TYPE_DOUBLE;
            outBind->buffer_length = sizeof (double);
            outBind->buffer        = data;
            break;

        case QMetaType::LongLong:
        case QMetaType::ULongLong:
            outBind->buffer_type   = MYSQL_TYPE_LONGLONG;
            outBind->buffer_length = sizeof (qint64);
            outBind->is_unsigned   = value.userType() == QMetaType::ULongLong;
            outBind->buffer        = data;
            break;

        case QMetaType::QString:
        // CUR drivers finish, test if is convertible to QString and do something with it silverqx
        default: {
            // Emplace to the vector to make it alive until mysql_stmt_execute()
            const auto &stringRef = stringVector.emplaceBack(value.toString().toUtf8());

            outBind->buffer_type   = MYSQL_TYPE_STRING;
            outBind->buffer_length = stringRef.size();
            outBind->buffer        = const_cast<char *>(stringRef.constData());
            break;
        }
        }
    }
}

//void MySqlResultPrivate::bindBlobs()
//{
//    for (int index = 0; index < fields.size(); ++index) {
//        const MYSQL_FIELD *const fieldInfo = fields.at(index).myField;

//        if (isBlobType(inBinds[index].buffer_type) && meta && fieldInfo) {
//            MYSQL_BIND *const bind = &inBinds[index];

//            bind->buffer_length = fieldInfo->max_length;

//            delete[] static_cast<char *>(bind->buffer);
//            bind->buffer = new char[fieldInfo->max_length];

//            fields[index].outField = static_cast<char *>(bind->buffer);
//        }
//    }
//}

/* Result sets */

QVariant MySqlResultPrivate::getValueForNormal(const int index) const
{
    const auto &field = fields.at(index);

    // Field is NULL
    if (row[index] == nullptr)
        return QVariant(field.type);

    // BIT field
    if (isBitType(field.myField->type))
        return QVariant::fromValue(toBitField(field, row[index]));

    QString value;
    quint64 fieldLength = 0;
    const auto typeId = field.type.id();
    // CUR drivers revisit silverqx
    fieldLength = mysql_fetch_lengths(result)[index];

    // BLOB field needs the QByteArray as the storage
    if (typeId != QMetaType::QByteArray)
        value = QString::fromUtf8(row[index], fieldLength);

    return createQVariant(typeId, std::move(value), index);
}

QVariant MySqlResultPrivate::getValueForPrepared(const int index) const
{
    const auto &field = fields.at(index);

    // Field is NULL
    if (field.nullIndicator)
        return QVariant(field.type);

    // BIT field
    if (isBitType(field.myField->type))
        return QVariant::fromValue(toBitField(field, field.outField));

    const auto typeId = field.type.id();

    // CUR drivers finish silverqx
    if (MySqlUtilsPrivate::isInteger(typeId)) {
        QVariant variant(field.type, field.outField);
        // we never want to return char variants here, see QTBUG-53397
        if (typeId == QMetaType::UChar)
            return variant.toUInt();
        if (typeId == QMetaType::Char)
            return variant.toInt();
        return variant;
    }

    // CUR drivers revisit this, how datetimes are returned from normal vs prepared queries because here is different logic than for normal queries silverqx
    if (MySqlUtils::isTimeOrDate(field.myField->type) &&
        field.bufferLength >= sizeof (QT_MYSQL_TIME)
    ) {
        const auto *const mysqlTime = reinterpret_cast<const QT_MYSQL_TIME *>(
                                          field.outField);
        QDate date;
        QTime time;

        if (typeId != QMetaType::QTime)
            date = QDate(mysqlTime->year, mysqlTime->month, mysqlTime->day);

        if (typeId != QMetaType::QDate)
            time = QTime(mysqlTime->hour, mysqlTime->minute, mysqlTime->second,
                         mysqlTime->second_part / 1000);

        if (typeId == QMetaType::QDateTime)
            return QDateTime(date, time);
        if (typeId == QMetaType::QDate)
            return date;
        if (typeId == QMetaType::QTime)
            return time;

        Q_UNREACHABLE();
    }

    QString value;

    // BLOB field needs the QByteArray as the storage
    if (typeId != QMetaType::QByteArray)
        value = QString::fromUtf8(field.outField, field.bufferLength);

    return createQVariant(typeId, std::move(value), index);
}

/* Others */

SqlDriverError
MySqlResultPrivate::makeStmtError(
        const QString &error, const SqlDriverError::ErrorType type,
        MYSQL_STMT *const stmt)
{
    const auto *const mysqlError = mysql_stmt_error(stmt);

    return SqlDriverError("QMYSQL: "_L1 + error, QString::fromUtf8(mysqlError),
                          type, QString::number(mysql_stmt_errno(stmt)));
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
            << QStringLiteral(
                   "MySqlResultPrivate::%1: fieldsCount != index, column "
                   "with the index '%2' has no metadata, this should never happen :/")
               .arg(method).arg(lastIndex);

    return false;
}

void MySqlResultPrivate::allocateMemoryForBindings(MYSQL_BIND **binds,
                                                   const std::size_t count) noexcept
{
    *binds = new MYSQL_BIND[count];
    // Zero the memory storage
    memset(*binds, 0, sizeof (MYSQL_BIND) * count);
}

void MySqlResultPrivate::reserveVectorsForBindings(
        QList<my_bool> &nullVector, QList<QByteArray> &stringVector,
        QList<QT_MYSQL_TIME> &timeVector) const
{
    nullVector.reserve(values.size());

    stringVector.reserve(std::ranges::count_if(values, [](const QVariant &value)
    {
        return Helpers::qVariantTypeId(value) == QMetaType::QString;
    }));

    timeVector.reserve(std::ranges::count_if(values, [](const QVariant &value)
    {
        const auto typeId = Helpers::qVariantTypeId(value);
        return typeId == QMetaType::QDateTime ||
               typeId == QMetaType::QDate     ||
               typeId == QMetaType::QTime;
    }));
}

bool MySqlResultPrivate::isBlobType(const enum_field_types fieldType)
{
    return fieldType == MYSQL_TYPE_TINY_BLOB   ||
           fieldType == MYSQL_TYPE_BLOB        ||
           fieldType == MYSQL_TYPE_MEDIUM_BLOB ||
           fieldType == MYSQL_TYPE_LONG_BLOB   ||
           fieldType == MYSQL_TYPE_JSON;
}

QT_MYSQL_TIME
MySqlResultPrivate::toMySqlDate(const QDate date, const QTime time, const int typeId,
                                MYSQL_BIND *const bind)
{
    Q_ASSERT((typeId == QMetaType::QTime || typeId == QMetaType::QDate ||
              typeId == QMetaType::QDateTime) &&
              bind != nullptr);

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
        bind->buffer_type = MYSQL_TYPE_DATETIME;
        break;

    case QMetaType::QDate:
        initDate();

        mysqlTime.time_type = MYSQL_TIMESTAMP_DATE;
        bind->buffer_type = MYSQL_TYPE_DATE;
        break;

    case QMetaType::QTime:
        initTime();

        mysqlTime.time_type = MYSQL_TIMESTAMP_TIME;
        bind->buffer_type = MYSQL_TYPE_TIME;
        break;

    default:
        Q_UNREACHABLE();
    }

    return mysqlTime;
}

/* Result sets */

// CUR drivers noexcept? silverqx
uint64_t MySqlResultPrivate::toBitField(const MyField &field, const char *const outField)
{
    // Byte-aligned length
    const auto numBytes = (field.myField->length + 7) / 8;
    quint64 result = 0;

    for (ulong index = 0; index < numBytes && outField; ++index) {
        quint64 tmp = static_cast<quint8>(outField[index]);
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
        return QVariant(QDate());

    return QVariant(QDate::fromString(value, Qt::ISODate));
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
        return QVariant(QTime());

    return QVariant(QTime::fromString(value, Qt::ISODate));
#else
    Q_UNUSED(value);
    return QVariant(value);
#endif
}

QVariant MySqlResultPrivate::toQDateTimeFromString(QString &&value)
{
#if QT_CONFIG(datestring)
    // Nothing to do
    if (value.isEmpty())
        return QVariant(QDateTime());

    // CUR drivers revisit and if it's true the also validate 14 digits string silverqx
    // TIMESTAMPS have the format yyyyMMddhhmmss
    if (value.size() == 14)
        value.insert(4, u'-').insert(7, u'-').insert(10, u'T').insert(13, u':')
             .insert(16, u':');

    return QVariant(QDateTime::fromString(value, Qt::ISODate));
#else
    Q_UNUSED(value);
    return QVariant(value);
#endif
}

QVariant MySqlResultPrivate::toQVariantDouble(QString &&value) const
{
    // CUR drivers use String::isNumber()? silverqx
    auto ok = false;
    // CUR drivers test this with the HighPrecision policy silverqx
    double valueDouble = value.toDouble(&ok);

    if (!ok) {
        qWarning().noquote()
            << QStringLiteral(
                   "MySqlResultPrivate::toQVariantDouble: unable to convert QString "
                   "to the double type, QString value is '%1'")
               .arg(value);
        return {};
    }

    switch(precisionPolicy) {
    // This is the default precision policy
    case QSql::LowPrecisionDouble:
        return QVariant(valueDouble);

    // The following two precision policies discard the fractional part silently
    case QSql::LowPrecisionInt32:
        return QVariant(valueDouble).toInt();
    case QSql::LowPrecisionInt64:
        return QVariant(valueDouble).toLongLong();

    case QSql::HighPrecision:
        return value;

    default:
        Q_UNREACHABLE();
    }
}

QVariant
MySqlResultPrivate::toQVariantByteArray(const int index) const
{
    if (preparedQuery) {
        const auto &field = fields.at(index);
        return {QByteArray(field.outField, field.bufferLength)};
    }

    // CUR drivers revisit this if I need to fetch the length silverqx
    const quint64 fieldLength = mysql_fetch_lengths(result)[index];
    return {QByteArray(row[index], fieldLength)};
}

QVariant MySqlResultPrivate::createQVariant(const int typeId, QString &&value,
                                            const int index) const
{
    switch (typeId) {
    case QMetaType::QString:
        return QVariant(value);

    case QMetaType::LongLong:
        return QVariant(value.toLongLong());

    case QMetaType::ULongLong:
        return QVariant(value.toULongLong());

    case QMetaType::Char:
    case QMetaType::Short:
    case QMetaType::Int:
        return QVariant(value.toInt());

    case QMetaType::UChar:
    case QMetaType::UShort:
    case QMetaType::UInt:
        return QVariant(value.toUInt());

    case QMetaType::QDateTime:
        return toQDateTimeFromString(std::move(value));

    case QMetaType::QDate:
        return toQDateFromString(value);

    case QMetaType::QTime:
        return toQTimeFromString(value);

    case QMetaType::Double:
        return toQVariantDouble(std::move(value));

    case QMetaType::QByteArray:
        return toQVariantByteArray(index);

    default:
        return QVariant(value);
    }

    Q_UNREACHABLE();
}

} // namespace Orm::Drivers::MySql

TINYORM_END_COMMON_NAMESPACE
