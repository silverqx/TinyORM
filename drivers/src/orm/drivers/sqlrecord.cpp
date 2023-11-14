#include "orm/drivers/sqlrecord.hpp"

#ifndef QT_NO_DEBUG_STREAM
#  include <QDebug>
#endif

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Drivers
{

/* public */

QVariant SqlRecord::value(const int index) const
{
    return m_fields.value(index).value();
}

QVariant SqlRecord::value(const QString &name) const
{
    return value(indexOf(name));
}

void SqlRecord::setValue(const int index, const QVariant &value)
{
    if (!contains(index))
        return;

    m_fields[index].setValue(value);
}

void SqlRecord::setValue(const QString &name, const QVariant &value)
{
    setValue(indexOf(name), value);
}

void SqlRecord::setNull(const int index)
{
    if (!contains(index))
        return;

    m_fields[index].clear();
}

void SqlRecord::setNull(const QString &name)
{
    setNull(indexOf(name));
}

bool SqlRecord::isNull(const int index) const
{
    return m_fields.value(index).isNull();
}

bool SqlRecord::isNull(const QString &name) const
{
    return isNull(indexOf(name));
}

QString SqlRecord::fieldName(const int index) const
{
    return m_fields.value(index).name();
}

int SqlRecord::indexOf(const QString &name) const
{
    QStringView tableName;
    QStringView fieldName(name);
    const qsizetype idx = name.indexOf(u'.');
    if (idx != -1) {
        tableName = fieldName.left(idx);
        fieldName = fieldName.mid(idx + 1);
    }
    const int cnt = count();
    for (int i = 0; i < cnt; ++i) {
        // Check the passed in name first in case it is an alias using a dot.
        // Then check if both the table and field match when there is a table name specified.
        const auto &currentField = m_fields.at(i);
        const auto &currentFieldName = currentField.name();
        if (currentFieldName.compare(name, Qt::CaseInsensitive) == 0 ||
            (idx != -1 &&
             currentFieldName.compare(fieldName, Qt::CaseInsensitive) == 0 &&
             currentField.tableName().compare(tableName, Qt::CaseInsensitive) == 0)
        ) {
            return i;
        }
    }
    return -1;
}

SqlField SqlRecord::field(const int index) const
{
    return m_fields.value(index);
}

SqlField SqlRecord::field(const QString &name) const
{
    return field(indexOf(name));
}

void SqlRecord::append(const SqlField &field)
{
    m_fields.append(field);
}

void SqlRecord::insert(const int index, const SqlField &field)
{
   m_fields.insert(index, field);
}

void SqlRecord::replace(const int index, const SqlField &field)
{
    // Nothing to replace
    if (!contains(index))
        return;

    m_fields[index] = field;
}

void SqlRecord::remove(const int index)
{
    // Nothing to remove
    if (!contains(index))
        return;

    m_fields.remove(index);
}

void SqlRecord::clear()
{
    m_fields.clear();
}

void SqlRecord::clearValues()
{
    for (auto &field : m_fields)
        field.clear();
}

} // namespace Orm::Drivers

TINYORM_END_COMMON_NAMESPACE

#ifndef QT_NO_DEBUG_STREAM
#  ifdef TINYORM_COMMON_NAMESPACE
QDebug operator<<(QDebug debug, const TINYORM_COMMON_NAMESPACE::Orm::Drivers::SqlRecord &record)
#  else
QDebug operator<<(QDebug debug, const Orm::Drivers::SqlRecord &record)
#  endif
#endif
{
    QDebugStateSaver saver(debug);
    debug.nospace();

    const auto count = record.count();
    debug << "SqlRecord(" << count << ')';

    for (auto index = 0; index < count; ++index) {
        debug.nospace();
        debug << '\n' << qSetFieldWidth(2) << Qt::right << index
              << Qt::left << qSetFieldWidth(0) << ':';

        debug.space();
        debug << record.field(index) << record.value(index).template value<QString>();
    }

    return debug;
}
