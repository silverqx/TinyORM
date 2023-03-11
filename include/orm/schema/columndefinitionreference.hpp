#pragma once
#ifndef ORM_SCHEMA_COLUMNDEFINITIONREFERENCE_HPP
#define ORM_SCHEMA_COLUMNDEFINITIONREFERENCE_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include "orm/schema/columndefinition.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::SchemaNs
{

    class ForeignIdColumnDefinitionReference;

    /*! Concept for the column reference return type, based on this template parameter
        will be decided what type it will be. */
    template<typename T>
    concept ColumnReferenceReturn = std::is_void_v<T> ||
                                    std::same_as<T, ForeignIdColumnDefinitionReference>;

    /*! Reference class to the ColumnDefinition, provides setters with a nice API
        for the database column (includes indexes). */
    template<ColumnReferenceReturn R = void>
    class ColumnDefinitionReference
    {
        // To access m_columnDefinition data member
        friend ForeignIdColumnDefinitionReference;

    public:
        /*! CRTP return type by the passed R template parameter. */
        using ColumnReferenceType =
                std::conditional_t<std::is_void_v<R>, ColumnDefinitionReference, R>;

        /*! Constructor. */
        ColumnDefinitionReference(ColumnDefinition &columnDefinition); // NOLINT(google-explicit-constructor)
        /*! Default destructor. */
        inline ~ColumnDefinitionReference() = default;

        /*! Copy constructor. */
        inline ColumnDefinitionReference(
                    const ColumnDefinitionReference &) = default;
        /*! Move constructor. */
        inline ColumnDefinitionReference(
                    ColumnDefinitionReference &&) noexcept = default;

        /*! Deleted copy assignment operator. */
        ColumnDefinitionReference &operator=(const ColumnDefinitionReference &) = delete;
        /*! Deleted move assignment operator. */
        ColumnDefinitionReference &operator=(ColumnDefinitionReference &&) = delete;

        /*! Place the column "after" another column (MySQL). */
        ColumnReferenceType &after(const QString &column);
        /*! Used as a modifier for generatedAs() (PostgreSQL). */
        ColumnReferenceType &always();
        /*! Set INTEGER column as auto-increment (primary key). */
        ColumnReferenceType &autoIncrement();
        /*! Specify a character set for the column (MySQL). */
        ColumnReferenceType &charset(const QString &charset);
        /*! Specify a collation for the column (MySQL/PostgreSQL/SQL Server). */
        ColumnReferenceType &collation(const QString &collation);
        /*! Add a comment to the column (MySQL/PostgreSQL). */
        ColumnReferenceType &comment(const QString &comment);
        /*! Specify a "default" value for the column. */
        ColumnReferenceType &defaultValue(const QVariant &value);
        /*! Place the column "first" in the table (MySQL). */
        ColumnReferenceType &first();
        /*! Set the starting value of an auto-incrementing field (MySQL/PostgreSQL),
            alias for the 'startingValue'. */
        ColumnReferenceType &from(int startingValue);
        /*! Create a SQL compliant identity column (PostgreSQL). */
        ColumnReferenceType &generatedAs(QString sequenceOptions = "");
        /*! Specify that the column should be invisible to "SELECT *" (MySQL). */
        ColumnReferenceType &invisible();
        /*! Determine whether to use the geography (default, false) or
            geometry type (PostgreSQL). */
        ColumnReferenceType &isGeometry();
        /*! Set the INTEGER column as UNSIGNED (MySQL). */
        ColumnReferenceType &isUnsigned();
        /*! Allow NULL values to be inserted into the column. */
        ColumnReferenceType &nullable(bool value = true);
        /*! The spatial reference identifier (SRID) of a geometry identifies the SRS
            in which the geometry is defined (MySQL/PostgreSQL), alias for the 'srid'. */
        ColumnReferenceType &projection(quint32 value);
        /*! The spatial reference identifier (SRID) of a geometry identifies the SRS
            in which the geometry is defined (MySQL/PostgreSQL). */
        ColumnReferenceType &srid(quint32 value);
        /*! Set the starting value of an auto-incrementing field (MySQL/PostgreSQL). */
        ColumnReferenceType &startingValue(int startingValue);
        /*! Create a stored generated column (MySQL/PostgreSQL/SQLite). */
        ColumnReferenceType &storedAs(QString expression);
        /*! Set the TIMESTAMP column to use CURRENT_TIMESTAMP as default value. */
        ColumnReferenceType &useCurrent();
        /*! Set the TIMESTAMP column to use CURRENT_TIMESTAMP when updating (MySQL). */
        ColumnReferenceType &useCurrentOnUpdate();
        /*! Create a virtual generated column (MySQL/PostgreSQL/SQLite). */
        ColumnReferenceType &virtualAs(QString expression);

        /*! Add an index. */
        ColumnReferenceType &index(const QString &indexName = "");
        /*! Add a primary index. */
        ColumnReferenceType &primary();
        /*! Add a fulltext index. */
        ColumnReferenceType &fulltext(const QString &indexName = "");
        /*! Add a spatial index. */
        ColumnReferenceType &spatialIndex(const QString &indexName = "");
        /*! Add a unique index. */
        ColumnReferenceType &unique(const QString &indexName = "");

    protected:
        /*! Reference to a column definition. */
        std::reference_wrapper<ColumnDefinition> m_columnDefinition;

    private:
        /*! Static cast this to a child's instance type (CRTP). */
        inline ColumnReferenceType &columnReference() noexcept;
    };

    /* I had to make this class templated to be able call eg. foreignId().nullable(),
       be able to call ColumnDefinitionReference methods
       on the ForeignIdColumnDefinitionReference class.
       ColumnDefinitionReference methods have to return
       the ForeignIdColumnDefinitionReference & if 'this' is
       the ForeignIdColumnDefinitionReference instance.
       It added a little mess but it was absolutely necessary.
       I can return ColumnReferenceType & because a real instance is created by
       Blueprint methods and lives in that fluent call, exception are
       ForeignKeyDefinitionReference methods, there & can not be returned because
       a real instance is created inside
       the ForeignIdColumnDefinitionReference::references() method so returning by
       a value is necessary (if not then dangling reference occurs and crash happens). */

    /* public */

    template<ColumnReferenceReturn R>
    ColumnDefinitionReference<R>::ColumnDefinitionReference(
        ColumnDefinition &columnDefinition
    )
        : m_columnDefinition(columnDefinition)
    {}

    template<ColumnReferenceReturn R>
    typename ColumnDefinitionReference<R>::ColumnReferenceType &
    ColumnDefinitionReference<R>::after(const QString &column)
    {
        m_columnDefinition.get().after = column;

        return columnReference();
    }

    template<ColumnReferenceReturn R>
    typename ColumnDefinitionReference<R>::ColumnReferenceType &
    ColumnDefinitionReference<R>::always()
    {
        m_columnDefinition.get().always = true;

        return columnReference();
    }

    template<ColumnReferenceReturn R>
    typename ColumnDefinitionReference<R>::ColumnReferenceType &
    ColumnDefinitionReference<R>::autoIncrement()
    {
        m_columnDefinition.get().autoIncrement = true;

        return columnReference();
    }

    template<ColumnReferenceReturn R>
    typename ColumnDefinitionReference<R>::ColumnReferenceType &
    ColumnDefinitionReference<R>::charset(const QString &charset)
    {
        m_columnDefinition.get().charset = charset;

        return columnReference();
    }

    template<ColumnReferenceReturn R>
    typename ColumnDefinitionReference<R>::ColumnReferenceType &
    ColumnDefinitionReference<R>::collation(const QString &collation)
    {
        m_columnDefinition.get().collation = collation;

        return columnReference();
    }

    template<ColumnReferenceReturn R>
    typename ColumnDefinitionReference<R>::ColumnReferenceType &
    ColumnDefinitionReference<R>::comment(const QString &comment)
    {
        m_columnDefinition.get().comment = comment;

        return columnReference();
    }

    template<ColumnReferenceReturn R>
    typename ColumnDefinitionReference<R>::ColumnReferenceType &
    ColumnDefinitionReference<R>::defaultValue(const QVariant &value)
    {
        m_columnDefinition.get().defaultValue = value;

        return columnReference();
    }

    template<ColumnReferenceReturn R>
    typename ColumnDefinitionReference<R>::ColumnReferenceType &
    ColumnDefinitionReference<R>::first()
    {
        m_columnDefinition.get().first = true;

        return columnReference();
    }

    template<ColumnReferenceReturn R>
    typename ColumnDefinitionReference<R>::ColumnReferenceType &
    ColumnDefinitionReference<R>::from(const int startingValue)
    {
        m_columnDefinition.get().from = startingValue;

        return columnReference();
    }

    template<ColumnReferenceReturn R>
    typename ColumnDefinitionReference<R>::ColumnReferenceType &
    ColumnDefinitionReference<R>::generatedAs(QString sequenceOptions)
    {
        m_columnDefinition.get().generatedAs = std::move(sequenceOptions);

        return columnReference();
    }

    template<ColumnReferenceReturn R>
    typename ColumnDefinitionReference<R>::ColumnReferenceType &
    ColumnDefinitionReference<R>::invisible()
    {
        m_columnDefinition.get().invisible = true;

        return columnReference();
    }

    template<ColumnReferenceReturn R>
    typename ColumnDefinitionReference<R>::ColumnReferenceType &
            ColumnDefinitionReference<R>::isGeometry()
    {
        m_columnDefinition.get().isGeometry = true;

        return columnReference();
    }

    template<ColumnReferenceReturn R>
    typename ColumnDefinitionReference<R>::ColumnReferenceType &
    ColumnDefinitionReference<R>::isUnsigned()
    {
        m_columnDefinition.get().isUnsigned = true;

        return columnReference();
    }

    template<ColumnReferenceReturn R>
    typename ColumnDefinitionReference<R>::ColumnReferenceType &
    ColumnDefinitionReference<R>::nullable(const bool value)
    {
        m_columnDefinition.get().nullable = value;

        return columnReference();
    }

    template<ColumnReferenceReturn R>
    typename ColumnDefinitionReference<R>::ColumnReferenceType &
    ColumnDefinitionReference<R>::projection(const quint32 value)
    {
        m_columnDefinition.get().srid = value;

        return columnReference();
    }

    template<ColumnReferenceReturn R>
    typename ColumnDefinitionReference<R>::ColumnReferenceType &
    ColumnDefinitionReference<R>::startingValue(const int startingValue)
    {
        m_columnDefinition.get().startingValue = startingValue;

        return columnReference();
    }

    template<ColumnReferenceReturn R>
    typename ColumnDefinitionReference<R>::ColumnReferenceType &
    ColumnDefinitionReference<R>::srid(const quint32 value)
    {
        m_columnDefinition.get().srid = value;

        return columnReference();
    }

    template<ColumnReferenceReturn R>
    typename ColumnDefinitionReference<R>::ColumnReferenceType &
    ColumnDefinitionReference<R>::storedAs(QString expression)
    {
        m_columnDefinition.get().storedAs = std::move(expression);

        return columnReference();
    }

    template<ColumnReferenceReturn R>
    typename ColumnDefinitionReference<R>::ColumnReferenceType &
    ColumnDefinitionReference<R>::useCurrent()
    {
        m_columnDefinition.get().useCurrent = true;

        return columnReference();
    }

    template<ColumnReferenceReturn R>
    typename ColumnDefinitionReference<R>::ColumnReferenceType &
    ColumnDefinitionReference<R>::useCurrentOnUpdate()
    {
        m_columnDefinition.get().useCurrentOnUpdate = true;

        return columnReference();
    }

    template<ColumnReferenceReturn R>
    typename ColumnDefinitionReference<R>::ColumnReferenceType &
    ColumnDefinitionReference<R>::virtualAs(QString expression)
    {
        m_columnDefinition.get().virtualAs = std::move(expression);

        return columnReference();
    }

    template<ColumnReferenceReturn R>
    typename ColumnDefinitionReference<R>::ColumnReferenceType &
    ColumnDefinitionReference<R>::index(const QString &indexName)
    {
        if (indexName.isEmpty())
            m_columnDefinition.get().index = true;
        else
            m_columnDefinition.get().index = indexName;

        return columnReference();
    }

    template<ColumnReferenceReturn R>
    typename ColumnDefinitionReference<R>::ColumnReferenceType &
    ColumnDefinitionReference<R>::primary()
    {
        m_columnDefinition.get().primary = true;

        return columnReference();
    }

    template<ColumnReferenceReturn R>
    typename ColumnDefinitionReference<R>::ColumnReferenceType &
    ColumnDefinitionReference<R>::fulltext(const QString &indexName)
    {
        if (indexName.isEmpty())
            m_columnDefinition.get().fulltext = true;
        else
            m_columnDefinition.get().fulltext = indexName;

        return columnReference();
    }

    template<ColumnReferenceReturn R>
    typename ColumnDefinitionReference<R>::ColumnReferenceType &
    ColumnDefinitionReference<R>::spatialIndex(const QString &indexName)
    {
        if (indexName.isEmpty())
            m_columnDefinition.get().spatialIndex = true;
        else
            m_columnDefinition.get().spatialIndex = indexName;

        return columnReference();
    }

    template<ColumnReferenceReturn R>
    typename ColumnDefinitionReference<R>::ColumnReferenceType &
    ColumnDefinitionReference<R>::unique(const QString &indexName)
    {
        if (indexName.isEmpty())
            m_columnDefinition.get().unique = true;
        else
            m_columnDefinition.get().unique = indexName;

        return columnReference();
    }

    /* private */

    template<ColumnReferenceReturn R>
    typename ColumnDefinitionReference<R>::ColumnReferenceType &
    ColumnDefinitionReference<R>::columnReference() noexcept
    {
        return static_cast<ColumnDefinitionReference<R>::ColumnReferenceType &>(*this);
    }

} // namespace Orm::SchemaNs

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_SCHEMA_COLUMNDEFINITIONREFERENCE_HPP
