#pragma once
#ifndef ORM_DRIVERS_SQLRESULT_HPP
#define ORM_DRIVERS_SQLRESULT_HPP

#include <orm/macros/systemheader.hpp>
TINY_SYSTEM_HEADER

#include <QtGlobal>

#include <memory>

#include "orm/drivers/driverstypes.hpp"
#include "orm/drivers/macros/export.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Drivers
{

    class SqlDriver;
    class SqlRecord;
    class SqlResultPrivate;

    /*! Database SQL result set abstract class. */
    class TINYDRIVERS_EXPORT SqlResult
    {
        Q_DISABLE_COPY_MOVE(SqlResult)
        Q_DECLARE_PRIVATE(SqlResult)

        // To access practically everything, all logic is defined in the SqlResult
        friend class SqlQuery;

    protected:
        /*! Constructor. */
        explicit SqlResult(std::unique_ptr<SqlResultPrivate> &&dd) noexcept;

    public:
        /*! Alias for the cursor, bound, and result values type. */
        using size_type = int;

        /*! Pure virtual destructor. */
        virtual ~SqlResult() = 0;

        /*! Get the low-level database result set handle (database specific). */
        virtual QVariant handle() const noexcept = 0;

        /*! Set the connection name. */
        void setConnectionName(QString &&connection) noexcept;

    protected:
        /*! Query placeholders syntax enum. */
        enum BindingSyntax {
            /*! Positional placeholders using the ? character. */
            PositionalBinding,
            /*! Named placeholders using eg. :id (not supported). */
            NamedBinding,
        };

        /* Getters / Setters */
        /*! Determine whether a cursor is positioned on a valid record/row. */
        bool isValid() const noexcept;

        /*! Get the current SQL query text. */
        QString query() const noexcept;
        /*! Set the query for the result. */
        void setQuery(QString query) noexcept;

        /*! Get the current cursor position (0-based). */
        size_type at() const noexcept;
        /*! Set the current cursor position (0-based). */
        virtual void setAt(size_type index) noexcept;

        /*! Determine whether the result set has records to be retrieved. */
        bool isActive() const noexcept;
        /*! Set a flag that this result is active (has records to be retrieved). */
        virtual void setActive(bool value) noexcept;

        /*! Determine whether the current result is from the SELECT statement. */
        bool isSelect() const noexcept;
        /*! Set a flag that the result is from the SELECT statement. */
        virtual void setSelect(bool value) noexcept;

        /*! Get the current numerical precision policy. */
        NumericalPrecisionPolicy numericalPrecisionPolicy() const noexcept;
        /*! Set the numerical precision policy for the current result. */
        void setNumericalPrecisionPolicy(NumericalPrecisionPolicy precision) noexcept;

        /*! Get the SQL database driver used to access the database connection (const). */
        const SqlDriver *driver() const noexcept;
        /*! Get the SQL database driver used to access the database connection (const). */
        std::weak_ptr<const SqlDriver> driverWeak() const noexcept;

        /* Normal queries */
        /*! Execute the given SQL query (non-prepared/normal only). */
        virtual bool exec(const QString &query) = 0;

        /* Prepared queries */
        /*! Prepare the given SQL query for execution. */
        virtual bool prepare(const QString &query) = 0;
        /*! Execute a previously prepared SQL query. */
        virtual bool exec() = 0;

        /*! Bound the positional placeholder value at the given index for the prepared
            statement. */
        virtual void bindValue(size_type index, const QVariant &value,
                               ParamType /*unused*/);
        /*! Bound the positional placeholder value at the given index for the prepared
            statement. */
        virtual void bindValue(size_type index, QVariant &&value, ParamType /*unused*/);

        /*! Add the placeholder value to the list of positional bound values. */
        void addBindValue(const QVariant &value, ParamType /*unused*/);
        /*! Add the placeholder value to the list of positional bound values. */
        void addBindValue(QVariant &&value, ParamType /*unused*/);

        /*! Get the placeholder value at the given index position. */
        QVariant boundValue(size_type index) const;

        /*! Get the number of bound values. */
        size_type boundValuesCount() const noexcept;

        /*! Get a QVariant vector of all bound values. */
        QVariantList boundValues() const noexcept;
        /*! Get a reference to the QVariant vector of all bound values. */
        QVariantList &boundValues() noexcept;

        /*! Clear all bound values. */
        void clearBoundValues();

        /*! Get the binding syntax for the current query (always PositionalBinding). */
        constexpr BindingSyntax bindingSyntax() const noexcept;

        /* Result sets */
        /*! Get a SqlRecord containing the field information for the current query. */
        virtual SqlRecord record() const = 0;
        /*! Get the ID of the most recent inserted row if the database supports it. */
        virtual QVariant lastInsertId() const = 0; // Can't be noexcept because PostgresResult::lastInsertId() will call driver()->createResult()

        /*! Retrieve the record at the given index and position the cursor on it. */
        virtual bool fetch(size_type index) = 0;
        /*! Retrieve the first record and position the cursor on it. */
        virtual bool fetchFirst() = 0;
        /*! Retrieve the last record and position the cursor on it. */
        virtual bool fetchLast() = 0;
        /*! Retrieve the next record and position the cursor on it. */
        virtual bool fetchNext() = 0;
        /*! Retrieve the previous record and position the cursor on it. */
        virtual bool fetchPrevious();

        /*! Get the field value at the given index in the current record. */
        virtual QVariant data(size_type index) = 0;
        /*! Determine whether the field at the given index is NULL. */
        virtual bool isNull(size_type index) = 0;

        /*! Get the size of the result (number of rows returned), -1 if the size can't be
            determined (database must support reporting about query size). */
        virtual size_type size() noexcept = 0;
        /*! Get the number of affected rows for DML queries or -1 if the size can't be
            determined. */
        virtual size_type numRowsAffected() = 0;

        /*! Release memory associated with the current result set. */
        virtual void detachFromResultSet() noexcept = 0;

        /*! Discard the current result set and navigate to the next if available
            (not supported). */
        virtual bool nextResult();

        /* Data members */
        /*! Smart pointer to the private implementation. */
        std::unique_ptr<SqlResultPrivate> d_ptr;

    private:
        /*! Get the database driver associated with the result. */
        std::weak_ptr<SqlDriver> driver() noexcept;
    };

    /* protected */

    /* Prepared queries */

    constexpr SqlResult::BindingSyntax SqlResult::bindingSyntax() const noexcept // NOLINT(readability-convert-member-functions-to-static)
    {
        return SqlResult::PositionalBinding;
    }

} // namespace Orm::Drivers

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_DRIVERS_SQLRESULT_HPP
