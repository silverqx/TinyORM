#include "orm/drivers/sqlfield.hpp"

#ifndef QT_NO_DEBUG_STREAM
#  include <QDebug>

#  include "orm/drivers/constants_p.hpp"
#endif

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Drivers
{

/* public */

SqlField::SqlField(const QString &fieldName, const QMetaType metaType,
                   const QString &table
)
    : m_value(QVariant(metaType, nullptr))
    , m_name(fieldName)
    , m_table(table)
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
    m_value = QVariant(m_metaType, nullptr);
}

/* Getters / Setters */

void SqlField::setMetaType(const QMetaType metaType)
{
    m_metaType = metaType;

    // CUR drivers revisit this and forbid invalid values? silverqx
    if (!m_value.isValid())
        m_value = QVariant(metaType, nullptr);
}

} // namespace Orm::Drivers

TINYORM_END_COMMON_NAMESPACE

#ifndef QT_NO_DEBUG_STREAM
using namespace Qt::StringLiterals;

namespace
{
    /*! Log the SqlField QVariant value to the QDebug stream. */
    inline void logSqlFieldValue(QDebug &debug, QVariant &&value)
    {
        using Orm::Drivers::Constants::null_;

        static const auto Invalid = "INVALID"_L1;
        static const auto Null = null_.toUpper();

        debug << ", value: ";

        // CUR drivers revisit this and forbid invalid values? silverqx
        if (!value.isValid())
            debug.noquote() << Invalid;

        else if (value.isNull())
            debug.noquote() << Null;
        else
            debug.quote() << value.template value<QString>();
    }
} // namespace

#  ifdef TINYORM_COMMON_NAMESPACE
QDebug
operator<<(QDebug debug, const TINYORM_COMMON_NAMESPACE::Orm::Drivers::SqlField &field)
#  else
QDebug operator<<(QDebug debug, const Orm::Drivers::SqlField &field)
#  endif
{
    using SqlFieldType = std::remove_cvref_t<decltype (field)>;

    QDebugStateSaver saver(debug);
    debug.nospace();

    debug << "SqlField(" << field.name() << ", " << field.metaType().name();

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

    // CUR drivers finish defaultValue() silverqx
    if (!field.defaultValue().isNull())
        debug << ", defaultValue: " << field.defaultValue();

    if (field.sqlType() >= 0)
        debug << ", sqlType: " << field.sqlType();

    debug << ", autoIncrement: " << field.isAutoIncrement()

          << ", tableName: "
          << (field.tableName().isEmpty() ? u"(not specified)"_s : field.tableName())
          << ')';

    return debug;
}
#endif
