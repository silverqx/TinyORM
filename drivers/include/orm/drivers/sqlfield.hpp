#pragma once
#ifndef ORM_DRIVERS_SQLFIELD_HPP
#define ORM_DRIVERS_SQLFIELD_HPP

#include <QVariant>

#include <orm/macros/commonnamespace.hpp>
#include <orm/macros/export.hpp>

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Drivers
{

    /*! The SqlField represents a database field. */
    class SHAREDLIB_EXPORT SqlField
    {
    public:
        /*! Determine whether a field is required during INSERT statements. */
        enum struct RequiredStatus {
            /*! Unknown required field status. */
            Unknown = -1,
            /*! Optional field. */
            Optional = 0,
            /*! Required field, INSERT will fail if a field doesn't have a value. */
            Required = 1,
        };

        /*! Expose the RequiredStatus enum. */
        using enum RequiredStatus;

        /*! Constructor. */
        explicit SqlField(
                const QString &fieldName = {}, QMetaType metaType = {},
                const QString &tableName = {});
        /*! Default destructor. */
        inline ~SqlField() = default;

        /*! Equality comparison operator for the SqlField. */
        inline bool operator==(const SqlField &) const = default;

        /*! Swap the SqlField. */
        void swap(SqlField &other) noexcept;

        /*! Clear the value of the field and set it to NULL. */
        void clear();

        /* Getters / Setters */
        /*! Determine whether the field QMetaType is valid (not value). */
        inline bool isValid() const;
        /*! Determine whether the field QVariant value is null. */
        inline bool isNull() const;

        /*! Get the field value. */
        inline QVariant value() const;
        /*! Set the field value. */
        inline void setValue(const QVariant &value);
        /*! Set the field value. */
        inline void setValue(QVariant &&value);

        /*! Get the field name. */
        inline QString name() const;
        /*! Set the field name. */
        inline void setName(const QString &name);
        /*! Set the field name. */
        inline void setName(QString &&name);

        /*! Get the table name a field belongs to. */
        inline QString tableName() const;
        /*! Set the table name for the current field. */
        inline void setTableName(const QString &tableName);
        /*! Set the table name for the current field. */
        inline void setTableName(QString &&tableName);

        /*! Get the field default value. */
        inline QVariant defaultValue() const;
        /*! Set the field default value. */
        inline void setDefaultValue(const QVariant &value);
        /*! Set the field default value. */
        inline void setDefaultValue(QVariant &&value);

        /*! Get the field QVariant value metatype. */
        inline QMetaType metaType() const;
        /*! Set the field QVariant value metatype. */
        void setMetaType(QMetaType metaType);

        /*! Determine whether a field is required. */
        inline bool isRequired() const;
        /*! Set the required field status. */
        inline void setRequired(bool required);

        /*! Determine whether a field is required. */
        inline RequiredStatus requiredStatus() const;
        /*! Set the required field status. */
        inline void setRequiredStatus(RequiredStatus status);

        /*! Get the field length. */
        inline qint64 length() const;
        /*! Set the field length. */
        inline void setLength(qint64 fieldLength);

        /*! Get the field precision. */
        inline qint64 precision() const;
        /*! Set the field precision. */
        inline void setPrecision(qint64 precision);

        /*! Get the underlying database field type (database dependent). */
        inline int typeID() const;
        /*! Get the underlying database field type (database dependent) (alias). */
        inline int sqlType() const;
        /*! Set the underlying database field type (database dependent). */
        inline void setSqlType(int sqlType);

        /*! Determine whether a field is auto-incrementing (alias). */
        inline bool isAutoIncrement() const;
        /*! Mark the field as auto-incrementing (alias). */
        inline void setAutoIncrement(bool value);

        /*! Determine whether a field is auto-incrementing. */
        inline bool isAutoValue() const;
        /*! Mark the field as auto-incrementing. */
        inline void setAutoValue(bool value);

    private:
        /* Data members */
        /*! Field value. */
        QVariant m_value;

        /*! Field name. */
        QString m_name;
        /*! Table name a field belongs to. */
        QString m_table;
        /*! Field default value. */
        QVariant m_defaultValue {};
        /*! Qt metatype. */
        QMetaType m_metaType;
        /*! Determine whether a field is required during INSERT statements. */
        SqlField::RequiredStatus m_requiredStatus = SqlField::Unknown;
        /*! Field length. */
        qint64 m_length = -1;
        /*! Field precision. */
        qint64 m_precision = -1;
        /*! Underlying database field type (DB dependent). */
        int m_sqlType = -1;
        /*! Determine whether a field is auto-incremented. */
        bool m_autoIncrement = false;
    };

    /* public */

    /* Getters / Setters */

    bool SqlField::isValid() const
    {
        return m_metaType.isValid();
    }

    bool SqlField::isNull() const
    {
        return m_value.isNull();
    }

    QVariant SqlField::value() const
    {
        return m_value;
    }

    void SqlField::setValue(const QVariant &value)
    {
        m_value = value;
    }

    void SqlField::setValue(QVariant &&value)
    {
        m_value = std::move(value);
    }

    QString SqlField::name() const
    {
        return m_name;
    }

    void SqlField::setName(const QString &name)
    {
        m_name = name;
    }

    void SqlField::setName(QString &&name)
    {
        m_name = std::move(name);
    }

    QString SqlField::tableName() const
    {
        return m_table;
    }

    void SqlField::setTableName(const QString &table)
    {
        m_table = table;
    }

    void SqlField::setTableName(QString &&table)
    {
        m_table = std::move(table);
    }

    QVariant SqlField::defaultValue() const
    {
        return m_defaultValue;
    }

    void SqlField::setDefaultValue(const QVariant &value)
    {
        m_defaultValue = value;
    }

    void SqlField::setDefaultValue(QVariant &&value)
    {
        m_defaultValue = std::move(value);
    }

    QMetaType SqlField::metaType() const
    {
        return m_metaType;
    }

    bool SqlField::isRequired() const
    {
        return m_requiredStatus == Required;
    }

    void SqlField::setRequired(const bool required)
    {
        setRequiredStatus(required ? Required : Optional);
    }

    SqlField::RequiredStatus SqlField::requiredStatus() const
    {
        return m_requiredStatus;
    }

    void SqlField::setRequiredStatus(const RequiredStatus required)
    {
        m_requiredStatus = required;
    }

    qint64 SqlField::length() const
    {
        return m_length;
    }

    void SqlField::setLength(const qint64 fieldLength)
    {
        m_length = fieldLength;
    }

    qint64 SqlField::precision() const
    {
        return m_precision;
    }

    void SqlField::setPrecision(const qint64 precision)
    {
        m_precision = precision;
    }

    int SqlField::typeID() const
    {
        return m_sqlType;
    }

    int SqlField::sqlType() const
    {
        return m_sqlType;
    }

    void SqlField::setSqlType(const int sqlType)
    {
        m_sqlType = sqlType;
    }

    bool SqlField::isAutoIncrement() const
    {
        return m_autoIncrement;
    }

    void SqlField::setAutoIncrement(const bool value)
    {
        m_autoIncrement = value;
    }

    bool SqlField::isAutoValue() const
    {
        return m_autoIncrement;
    }

    void SqlField::setAutoValue(const bool value)
    {
        m_autoIncrement = value;
    }

} // namespace Orm::Drivers

TINYORM_END_COMMON_NAMESPACE

#ifndef QT_NO_DEBUG_STREAM
#  ifdef TINYORM_COMMON_NAMESPACE
SHAREDLIB_EXPORT QDebug
operator<<(QDebug debug, const TINYORM_COMMON_NAMESPACE::Orm::Drivers::SqlField &field);
#  else
SHAREDLIB_EXPORT QDebug operator<<(QDebug debug, const Orm::Drivers::SqlField &field);
#  endif
#endif

#endif // ORM_DRIVERS_SQLFIELD_HPP

// CUR drivers check/change to int vs quint64 everywhere silverqx
// CUR drivers add noexcept everywhere silverqx
