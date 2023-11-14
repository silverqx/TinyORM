#include "orm/drivers/sqlfield.hpp"

#ifndef QT_NO_DEBUG_STREAM
#  include <QDebug>
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

void SqlField::clear()
{
    m_value = QVariant(m_metaType, nullptr);
}

/* Getters / Setters */

void SqlField::setMetaType(const QMetaType metaType)
{
    m_metaType = metaType;

    if (!m_value.isValid())
        m_value = QVariant(metaType, nullptr);
}

} // namespace Orm::Drivers

TINYORM_END_COMMON_NAMESPACE

#ifndef QT_NO_DEBUG_STREAM
#  ifdef TINYORM_COMMON_NAMESPACE
QDebug operator<<(QDebug debug, const TINYORM_COMMON_NAMESPACE::Orm::Drivers::SqlField &field)
#  else
QDebug operator<<(QDebug debug, const Orm::Drivers::SqlField &field)
#  endif
{
    using SqlFieldType = std::remove_cvref_t<decltype (field)>;

    QDebugStateSaver saver(debug);
    debug.nospace();

    debug << "SqlField(" << field.name() << ", " << field.metaType().name();
    debug << ", tableName: "
          << (field.tableName().isEmpty() ? QStringLiteral("(empty or null)")
                                          : field.tableName());
    if (field.length() >= 0)
        debug << ", length: " << field.length();

    if (field.precision() >= 0)
        debug << ", precision: " << field.precision();

    if (field.requiredStatus() != SqlFieldType::Unknown)
        debug << ", required: "
              << (field.requiredStatus() == SqlFieldType::Required ? "yes" : "no");

    if (field.typeID() >= 0)
        debug << ", typeID: " << field.typeID();

    if (!field.defaultValue().isNull())
        debug << ", defaultValue: \"" << field.defaultValue() << '\"';

    debug << ", autoValue: " << field.isAutoValue() << ')';

    return debug;
}
#endif
