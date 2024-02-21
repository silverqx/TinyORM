#include "orm/drivers/sqlfield.hpp"

#ifndef QT_NO_DEBUG_STREAM
#  include <QDebug>

#  include "orm/drivers/constants_p.hpp"
#endif

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Drivers
{

/* public */

SqlField::SqlField(QString fieldName, const QMetaType metaType, QString table)
    : m_value(QVariant(metaType))
    , m_name(std::move(fieldName))
    , m_table(std::move(table))
    , m_metaType(metaType)
{}

void SqlField::swap(SqlField &other) noexcept
{
    m_value.swap(other.m_value);
    m_name.swap(other.m_name);
    m_table.swap(other.m_table);
    m_defaultValue.swap(other.m_defaultValue);

    std::swap(m_metaType, other.m_metaType);
    std::swap(m_requiredStatus, other.m_requiredStatus);
    std::swap(m_length, other.m_length);
    std::swap(m_precision, other.m_precision);
    std::swap(m_sqlType, other.m_sqlType);
    std::swap(m_autoIncrement, other.m_autoIncrement);
}

void SqlField::clear()
{
    m_value = QVariant(m_metaType);
}

/* Getters / Setters */

void SqlField::setMetaType(const QMetaType metaType)
{
    m_metaType = metaType;

    if (!m_value.isValid())
        m_value = QVariant(metaType);
}

} // namespace Orm::Drivers

TINYORM_END_COMMON_NAMESPACE

#ifndef QT_NO_DEBUG_STREAM
using namespace Qt::StringLiterals; // NOLINT(google-build-using-namespace)

namespace
{
    /*! Log the SqlField QVariant value to the QDebug stream. */
    inline void logSqlFieldValue(QDebug &debug, const QVariant &value)
    {
        using Orm::Drivers::Constants::null_;

        static const auto Invalid = "INVALID"_L1;
        static const auto Null = null_.toUpper();

        debug << ", value: ";

        // isValid() must be checked first because an invalid QVariant is also null
        if (!value.isValid())
            debug.noquote() << Invalid;

        else if (value.isNull())
            debug.noquote() << Null;
        else
            debug.quote() << value.template value<QString>();
    }
} // namespace

QDebug
operator<<(QDebug debug, const TINYORM_PREPEND_NAMESPACE(Orm::Drivers::SqlField) &field)
{
    using SqlFieldType = std::remove_cvref_t<decltype (field)>;

    const QDebugStateSaver saver(debug);
    debug.nospace();

    debug << "SqlField(name: " << field.name()
          << ", type: " << field.metaType().name();

    // Log the SqlField QVariant value to the QDebug stream
    logSqlFieldValue(debug, field.value());

    debug.quote() << ", isNull: "  << field.isNull();
    debug.quote() << ", isValid: " << field.isValid();

    if (field.length() >= 0)
        debug << ", length: " << field.length();

    if (field.precision() >= 0)
        debug << ", precision: " << field.precision();

    if (field.requiredStatus() != SqlFieldType::Unknown)
        debug << ", required: " << field.isRequired();

    // CUR drivers finish defaultValue(), look at the bottom of this file for more info silverqx
    if (!field.defaultValue().isNull())
        debug << ", defaultValue: " << field.defaultValue();

    // FUTURE drivers MySQL in client.cc exists fieldtype2str() that converts field.sqlType() to string, add support for printing this to SqlField; a new data member with getter/setter will be needed silverqx
    if (field.sqlType() >= 0)
        debug << ", sqlType: " << field.sqlType();

    debug << ", autoIncrement: " << field.isAutoIncrement()

          << ", tableName: "
          << (field.tableName().isEmpty() ? u"(not specified)"_s : field.tableName())
          << ')';

    return debug;
}
#endif

/* CUR drivers finish defaultValue():
   We will need something like this, also the MYSQL_FIELD.def data member was dropped
   in MySQL v8.3, what means connector C API doesn't provide this default field value,
   also both IS_NULLABLE and COLUMN_DEFAULT must be checked to correctly obtain this
   information, there is also some info at about NO_DEFAULT_VALUE_FLAG:
   https://dev.mysql.com/doc/c-api/8.0/en/c-api-data-structures.html:
- SHOW COLUMNS FROM users;
- describe users; (the same as SHOW COLUMNS)
- select c.COLUMN_NAME, c.IS_NULLABLE, c.COLUMN_DEFAULT from information_schema.`COLUMNS` c where c.TABLE_SCHEMA = 'tinyorm_test_1' and c.TABLE_NAME = 'users'; (this select-s as little info as possible)
*/
