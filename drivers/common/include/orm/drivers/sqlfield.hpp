#pragma once
#ifndef ORM_DRIVERS_SQLFIELD_HPP
#define ORM_DRIVERS_SQLFIELD_HPP

#include <orm/macros/systemheader.hpp>
TINY_SYSTEM_HEADER

#include <QVariant>

#include <orm/macros/commonnamespace.hpp>

#include "orm/drivers/macros/export.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Drivers
{

    /*! The SqlField represents a database field. */
    class TINYDRIVERS_EXPORT SqlField
    {
    public:
        /*! Alias for the SqlRecord size type. */
        using size_type = qint64;

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
        explicit SqlField(QString fieldName = {}, QMetaType metaType = {},
                          QString tableName = {});
        /*! Default destructor. */
        inline ~SqlField() = default;

        /*! Copy constructor. */
        inline SqlField(const SqlField &) = default;
        /*! Copy assignment operator. */
        inline SqlField &operator=(const SqlField &) = default;

        /*! Move constructor. */
        inline SqlField(SqlField &&) noexcept = default;
        /*! Move assignment operator. */
        inline SqlField &operator=(SqlField &&) noexcept = default;

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
        inline void setValue(QVariant &&value) noexcept;

        /*! Get the field name. */
        inline QString name() const noexcept;
        /*! Set the field name. */
        inline void setName(const QString &name) noexcept;
        /*! Set the field name. */
        inline void setName(QString &&name) noexcept;

        /*! Get the table name a field belongs to. */
        inline QString tableName() const noexcept;
        /*! Set the table name for the current field. */
        inline void setTableName(const QString &tableName) noexcept;
        /*! Set the table name for the current field. */
        inline void setTableName(QString &&tableName) noexcept;

        /*! Get the field default value. */
        inline QVariant defaultValue() const;
        /*! Set the field default value. */
        inline void setDefaultValue(const QVariant &value);
        /*! Set the field default value. */
        inline void setDefaultValue(QVariant &&value) noexcept;

        /*! Get the field QVariant value metatype. */
        inline QMetaType metaType() const noexcept;
        /*! Set the field QVariant value metatype (for invalid values only). */
        void setMetaType(QMetaType metaType);

        /*! Determine whether a field is required. */
        inline bool isRequired() const noexcept;
        /*! Set the required field status. */
        inline void setRequired(bool required) noexcept;

        /*! Determine whether a field is required. */
        inline RequiredStatus requiredStatus() const noexcept;
        /*! Set the required field status. */
        inline void setRequiredStatus(RequiredStatus required) noexcept;

        /*! Get the field length. */
        inline qint64 length() const noexcept;
        /*! Set the field length. */
        inline void setLength(size_type fieldLength) noexcept;

        /*! Get the field precision. */
        inline qint64 precision() const noexcept;
        /*! Set the field precision. */
        inline void setPrecision(size_type precision) noexcept;

        /*! Get the underlying database field type (database dependent). */
        inline int typeID() const noexcept;
        /*! Get the underlying database field type (database dependent) (alias). */
        inline int sqlType() const noexcept;
        /*! Set the underlying database field type (database dependent). */
        inline void setSqlType(int sqlType) noexcept;

        /*! Get the underlying database field type (database dependent) (alias). */
        inline QString sqlTypeName() const noexcept;
        /*! Set the underlying database field type (database dependent). */
        inline void setSqlTypeName(const QString &sqlTypeName) noexcept;
        /*! Set the underlying database field type (database dependent). */
        inline void setSqlTypeName(QString &&sqlTypeName) noexcept;

        /*! Determine whether a field is auto-incrementing (alias). */
        inline bool isAutoIncrement() const noexcept;
        /*! Mark the field as auto-incrementing (alias). */
        inline void setAutoIncrement(bool value) noexcept;

        /*! Determine whether a field is auto-incrementing. */
        inline bool isAutoValue() const noexcept;
        /*! Mark the field as auto-incrementing. */
        inline void setAutoValue(bool value) noexcept;

    private:
        /* Data members */
        /*! Field value. */
        QVariant m_value;

        /*! Field name. */
        QString m_name;
        /*! Table name a field belongs to. */
        QString m_table;
        /*! Underlying database field type name (DB dependent). */
        QString m_sqlTypeName {}; // Moved up here for a better memory layout/padding
        /*! Field default value. */
        QVariant m_defaultValue {};
        /*! Qt metatype. */
        QMetaType m_metaType;
        /*! Determine whether a field is required during INSERT statements. */
        SqlField::RequiredStatus m_requiredStatus = SqlField::Unknown;
        /*! Field length. */
        size_type m_length = -1;
        /*! Field precision. */
        size_type m_precision = -1;
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

    void SqlField::setValue(QVariant &&value) noexcept
    {
        m_value = std::move(value);
    }

    QString SqlField::name() const noexcept
    {
        return m_name;
    }

    void SqlField::setName(const QString &name) noexcept
    {
        m_name = name;
    }

    void SqlField::setName(QString &&name) noexcept
    {
        m_name = std::move(name);
    }

    QString SqlField::tableName() const noexcept
    {
        return m_table;
    }

    void SqlField::setTableName(const QString &table) noexcept
    {
        m_table = table;
    }

    void SqlField::setTableName(QString &&table) noexcept
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

    void SqlField::setDefaultValue(QVariant &&value) noexcept
    {
        m_defaultValue = std::move(value);
    }

    QMetaType SqlField::metaType() const noexcept
    {
        return m_metaType;
    }

    bool SqlField::isRequired() const noexcept
    {
        return m_requiredStatus == Required;
    }

    void SqlField::setRequired(const bool required) noexcept
    {
        setRequiredStatus(required ? Required : Optional);
    }

    SqlField::RequiredStatus SqlField::requiredStatus() const noexcept
    {
        return m_requiredStatus;
    }

    void SqlField::setRequiredStatus(const RequiredStatus required) noexcept
    {
        m_requiredStatus = required;
    }

    qint64 SqlField::length() const noexcept
    {
        return m_length;
    }

    void SqlField::setLength(const size_type fieldLength) noexcept
    {
        m_length = fieldLength;
    }

    qint64 SqlField::precision() const noexcept
    {
        return m_precision;
    }

    void SqlField::setPrecision(const size_type precision) noexcept
    {
        m_precision = precision;
    }

    int SqlField::typeID() const noexcept
    {
        return m_sqlType;
    }

    int SqlField::sqlType() const noexcept
    {
        return m_sqlType;
    }

    void SqlField::setSqlType(const int sqlType) noexcept
    {
        m_sqlType = sqlType;
    }

    QString SqlField::sqlTypeName() const noexcept
    {
        return m_sqlTypeName;
    }

    void SqlField::setSqlTypeName(const QString &sqlTypeName) noexcept
    {
        m_sqlTypeName = sqlTypeName;
    }

    void SqlField::setSqlTypeName(QString &&sqlTypeName) noexcept
    {
        m_sqlTypeName = std::move(sqlTypeName);
    }

    bool SqlField::isAutoIncrement() const noexcept
    {
        return m_autoIncrement;
    }

    void SqlField::setAutoIncrement(const bool value) noexcept
    {
        m_autoIncrement = value;
    }

    bool SqlField::isAutoValue() const noexcept
    {
        return m_autoIncrement;
    }

    void SqlField::setAutoValue(const bool value) noexcept
    {
        m_autoIncrement = value;
    }

} // namespace Orm::Drivers

TINYORM_END_COMMON_NAMESPACE

#ifndef QT_NO_DEBUG_STREAM
TINYDRIVERS_EXPORT QDebug
operator<<(QDebug debug, const TINYORM_PREPEND_NAMESPACE(Orm::Drivers::SqlField) &field);
#endif

#endif // ORM_DRIVERS_SQLFIELD_HPP
