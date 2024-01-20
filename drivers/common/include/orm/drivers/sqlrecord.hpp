#pragma once
#ifndef ORM_DRIVERS_SQLRECORD_HPP
#define ORM_DRIVERS_SQLRECORD_HPP

#include <orm/macros/systemheader.hpp>
TINY_SYSTEM_HEADER

#include "orm/drivers/sqlfield.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Drivers
{

    /*! The SqlRecord represents a database row. */
    class TINYDRIVERS_EXPORT SqlRecord
    {
    public:
        /* Container related */
        /*! Alias for the SqlRecord size type. */
        using size_type = int;

        /*! Default constructor. */
        inline SqlRecord() = default;
        /*! Default destructor. */
        inline ~SqlRecord() = default;

        /*! Copy constructor. */
        inline SqlRecord(const SqlRecord &) = default;
        /*! Copy assignment operator. */
        inline SqlRecord &operator=(const SqlRecord &) = default;

        /*! Move constructor. */
        inline SqlRecord(SqlRecord &&) noexcept = default;
        /*! Move assignment operator. */
        inline SqlRecord &operator=(SqlRecord &&) noexcept = default;

        /*! Equality comparison operator for the SqlRecord. */
        inline bool operator==(const SqlRecord &) const = default;

        /*! Swap the SqlRecord. */
        inline void swap(SqlRecord &other) noexcept;

        /*! Get the field name at the given index. */
        QString fieldName(size_type index) const;
        /*! Get the index of the given field name (-1 if it can't be found). */
        size_type indexOf(const QString &name) const;

        /*! Get the field at the given index. */
        SqlField field(size_type index) const;
        /*! Get the field by field name. */
        SqlField field(const QString &name) const;

        /*! Get the field value at the given index. */
        QVariant value(size_type index) const;
        /*! Get the field value by field name. */
        QVariant value(const QString &name) const;

        /*! Set the value of the field at the given index to the given value. */
        void setValue(size_type index, const QVariant &value);
        /*! Set the value of the field at the given index to the given value. */
        void setValue(size_type index, QVariant &&value);

        /*! Set the value of the field with the field name to the given value. */
        void setValue(const QString &name, const QVariant &value);
        /*! Set the value of the field with the field name to the given value. */
        void setValue(const QString &name, QVariant &&value);

        /*! Determine whether the field at the given index is NULL. */
        bool isNull(size_type index) const;
        /*! Determine whether the field with the given field name is NULL. */
        bool isNull(const QString &name) const;

        /*! Set the value of the field at the given index to NULL. */
        void setNull(size_type index);
        /*! Set the value of the field with the given field name to NULL. */
        void setNull(const QString &name);

        /*! Append a copy of the given field to the end of the record. */
        void append(const SqlField &field);
        /*! Append the given field to the end of the record. */
        void append(SqlField &&field);

        /*! Insert a copy of the given field at the index position in the record. */
        void insert(size_type index, const SqlField &field);
        /*! Insert the given field at the index position in the record. */
        void insert(size_type index, SqlField &&field);

        /*! Replace the field at the index position with the given field. */
        void replace(size_type index, const SqlField &field);
        /*! Replace the field at the index position with the given field. */
        void replace(size_type index, SqlField &&field);

        /*! Remove the field at the given index position from the record. */
        void remove(size_type index);

        /*! Removes all field from the record. */
        void clear();
        /*! Clear all field values in the record and set each field to NULL. */
        void clearValues();

        /*! Determine whether the current record contains fields. */
        inline bool isEmpty() const;
        /*! Get the number of fields in the current record. */
        inline size_type count() const;

        /*! Determine whether the current record contains the given field name. */
        inline bool contains(const QString &name) const;
        /*! Determine whether the current record contains the given field index. */
        inline bool contains(size_type index) const;

    private:
        /*! Throw the std::out_of_range() exception if record doesn't contain an index. */
        void throwIfNotContains(size_type index, const QString &functionName) const;

        /*! FieldSegmentsType return type for getFieldNameSegments(). */
        struct FieldSegmentsType
        {
            /*! Determine whether a given field is qualified. */
            bool isQualifiedName = false;
            /*! Table name. */
            QStringView tableName;
            /*! Field name. */
            QStringView fieldName;
        };

        /*! Get individual segments from the aliased field identifier
            (column alias (select expression)). */
        static FieldSegmentsType getFieldNameSegments(QStringView name);

        /* Data members */
        /*! Record fields. */
        QList<SqlField> m_fields;
    };

    /* public */

    void SqlRecord::swap(SqlRecord &other) noexcept
    {
        m_fields.swap(other.m_fields);
    }

    bool SqlRecord::isEmpty() const
    {
        return m_fields.isEmpty();
    }

    SqlRecord::size_type SqlRecord::count() const
    {
        return static_cast<size_type>(m_fields.size());
    }

    bool SqlRecord::contains(const QString &name) const
    {
        return indexOf(name) >= 0;
    }

    bool SqlRecord::contains(const size_type index) const
    {
      return index >= 0 && index < m_fields.size();
    }

} // namespace Orm::Drivers

TINYORM_END_COMMON_NAMESPACE

#ifndef QT_NO_DEBUG_STREAM
TINYDRIVERS_EXPORT QDebug
operator<<(QDebug debug, const TINYORM_PREPEND_NAMESPACE(Orm::Drivers::SqlRecord) &record);
#endif

#endif // ORM_DRIVERS_SQLRECORD_HPP
