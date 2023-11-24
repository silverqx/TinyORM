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
        enum RequiredStatus {
            Unknown = -1,
            Optional = 0,
            Required = 1,
        };

        /*! Constructor. */
        explicit SqlField(
                const QString &fieldName = {}, QMetaType metaType = {},
                const QString &tableName = {});
        /*! Default destructor. */
        inline ~SqlField() = default;

        /*! Equality comparison operator for the SqlField. */
        inline bool operator==(const SqlField &) const = default;

        // CUR drivers swap silverqx

        void clear();

        /* Getters / Setters */
        /*! Determine whether the field QVariant value is valid. */
        inline bool isValid() const;
        /*! Determine whether the field QVariant value is null. */
        inline bool isNull() const;

        /*! Get the field value. */
        inline QVariant value() const;
        /*! Set the field value. */
        inline void setValue(const QVariant &value);

        /*! Get the field name. */
        inline QString name() const;
        /*! Set the field name. */
        inline void setName(const QString &name);

        /*! Get the table name a field belongs to. */
        inline QString tableName() const;
        /*! Set the table name for the current field. */
        inline void setTableName(const QString &tableName);

        /*! Get the field default value. */
        inline QVariant defaultValue() const;
        /*! Set the field default value. */
        inline void setDefaultValue(const QVariant &value);

        /*! Get the field QVariant value metatype. */
        inline QMetaType metaType() const;
        /*! Set the field QVariant value metatype. */
        void setMetaType(QMetaType metaType);

        inline RequiredStatus requiredStatus() const;
        inline void setRequiredStatus(RequiredStatus status);
        inline void setRequired(bool required);

        /*! Get the field length. */
        inline int length() const;
        /*! Set the field length. */
        inline void setLength(int fieldLength);

        /*! Get the field precision. */
        inline int precision() const;
        /*! Set the field precision. */
        inline void setPrecision(int precision);

        /*! Get the underlying database field type (DB dependant). */
        inline int typeID() const;
        /*! Get the underlying database field type (DB dependant). */
        inline int sqlType() const;
        /*! Set the underlying database field type (DB dependant). */
        inline void setSqlType(int sqlType);

        /*! Determine whether a field is auto-incremented. */
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
        /*! Qt metaType. */
        QMetaType m_metaType;
        /*! Determine whether a field is required during INSERT statements. */
        SqlField::RequiredStatus m_requiredStatus = SqlField::Unknown;
        /*! Field length. */
        int m_length = -1;
        /*! Field precision. */
        int m_precision = -1;
        /*! Underlying database field type (DB dependant). */
        int m_sqlType = -1;
        /*! Determine whether a field is auto-incremented. */
        bool m_autovalue = false;
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

    QString SqlField::name() const
    {
        return m_name;
    }

    void SqlField::setName(const QString &name)
    {
        m_name = name;
    }

    QString SqlField::tableName() const
    {
        return m_table;
    }

    void SqlField::setTableName(const QString &table)
    {
        m_table = table;
    }

    QVariant SqlField::defaultValue() const
    {
        return m_defaultValue;
    }

    void SqlField::setDefaultValue(const QVariant &value)
    {
        m_defaultValue = value;
    }

    QMetaType SqlField::metaType() const
    {
        return m_metaType;
    }

    SqlField::RequiredStatus SqlField::requiredStatus() const
    {
        return m_requiredStatus;
    }

    void SqlField::setRequiredStatus(const RequiredStatus required)
    {
        m_requiredStatus = required;
    }

    void SqlField::setRequired(const bool required)
    {
        setRequiredStatus(required ? Required : Optional);
    }

    int SqlField::length() const
    {
        return m_length;
    }

    void SqlField::setLength(const int fieldLength)
    {
        m_length = fieldLength;
    }

    int SqlField::precision() const
    {
        return m_precision;
    }

    void SqlField::setPrecision(const int precision)
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

    bool SqlField::isAutoValue() const
    {
        return m_autovalue;
    }

    void SqlField::setAutoValue(const bool value)
    {
        m_autovalue = value;
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
