#pragma once
#ifndef ORM_DRIVERS_SQLRECORD_HPP
#define ORM_DRIVERS_SQLRECORD_HPP

#include "orm/drivers/sqlfield.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Drivers
{

    /*! The SqlRecord represents a database record or row. */
    class SHAREDLIB_EXPORT SqlRecord
    {
    public:
        /*! Default constructor. */
        inline SqlRecord() = default;
        /*! Default destructor. */
        inline ~SqlRecord() = default;

        /*! Equality comparison operator for the SqlRecord. */
        inline bool operator==(const SqlRecord &) const = default;

        // CUR drivers swap silverqx

        QVariant value(int index) const;
        QVariant value(const QString &name) const;
        void setValue(int index, const QVariant &value);
        void setValue(const QString &name, const QVariant &value);

        void setNull(int index);
        void setNull(const QString &name);
        bool isNull(int index) const;
        bool isNull(const QString &name) const;

        QString fieldName(int index) const;
        int indexOf(const QString &name) const;

        SqlField field(int index) const;
        SqlField field(const QString &name) const;

        void append(const SqlField &field);
        void insert(int index, const SqlField &field);
        void replace(int index, const SqlField &field);
        void remove(int index);

        void clear();
        void clearValues();

        inline bool isEmpty() const;
        inline int count() const;
        inline bool contains(const QString &name) const;

    private:
        inline bool contains(qsizetype index) const;

        /* Data members */
        QList<SqlField> m_fields;
    };

    /* public */

    bool SqlRecord::isEmpty() const
    {
        return m_fields.isEmpty();
    }

    int SqlRecord::count() const
    {
        return m_fields.size();
    }

    bool SqlRecord::contains(const QString &name) const
    {
        return indexOf(name) >= 0;
    }

    /* private */

    bool SqlRecord::contains(const qsizetype index) const
    {
      return index >= 0 && index < m_fields.size();
    }

} // namespace Orm::Drivers

TINYORM_END_COMMON_NAMESPACE

#ifndef QT_NO_DEBUG_STREAM
#  ifdef TINYORM_COMMON_NAMESPACE
SHAREDLIB_EXPORT QDebug operator<<(QDebug debug, const TINYORM_COMMON_NAMESPACE::Orm::Drivers::SqlRecord &record);
#  else
SHAREDLIB_EXPORT QDebug operator<<(QDebug debug, const Orm::Drivers::SqlRecord &record);
#  endif
#endif

#endif // ORM_DRIVERS_SQLRECORD_HPP
