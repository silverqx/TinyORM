#pragma once
#ifndef ORM_DRIVERS_SQLQUERY_HPP
#define ORM_DRIVERS_SQLQUERY_HPP

#include <orm/macros/systemheader.hpp>
TINY_SYSTEM_HEADER

#include "orm/drivers/sqlresult.hpp"
#include "orm/drivers/utils/notnull.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Drivers
{

    class SqlDatabase;
    class SqlDriver;
    class SqlError;
    class SqlRecord;
    class SqlQueryPrivate;

    /*! SqlQuery class executes, navigates, and retrieves data from SQL statements. */
    class TINYDRIVERS_EXPORT SqlQuery
    {
        Q_DISABLE_COPY(SqlQuery)

        /*! Alias for the NotNull. */
        template<typename T>
        using NotNull = Orm::Drivers::Utils::NotNull<T>;

    public:
        /*! Alias for the cursor, bound, and result values type. */
        using size_type = SqlResult::size_type;

        /*! Default constructor. */
        SqlQuery();
        /*! Converting constructor from the SqlDatabase. */
        explicit SqlQuery(const SqlDatabase &connection);
        /*! Converting constructor from the std::unique_ptr<SqlResult>. */
        explicit SqlQuery(std::unique_ptr<SqlResult> &&result) noexcept;

        /*! Move constructor. */
        inline SqlQuery(SqlQuery &&) noexcept = default;
        /*! Move assignment operator. */
        inline SqlQuery &operator=(SqlQuery &&) noexcept = default;

        /*! Default destructor. */
        ~SqlQuery();

        /*! Swap the SqlQuery. */
        constexpr void swap(SqlQuery &other) noexcept;

        /* Getters / Setters */
        /*! Determine whether the query result set is positioned on a valid record. */
        bool isValid() const noexcept;

        /*! Get the last executed query. */
        QString executedQuery() const noexcept;
        /*! Get the last executed query (alias). */
        QString lastQuery() const noexcept;
        /*! Get information about the error of the last query. */
        SqlError lastError() const noexcept;

        /*! Get the current cursor position (0-based). */
        size_type at() const noexcept;

        /*! Determine if the query was exec()'d successfully and isn't yet finished. */
        bool isActive() const noexcept;
        /*! Determine whether the current query is a SELECT statement. */
        bool isSelect() const noexcept;

        /*! Get the current numerical precision policy. */
        NumericalPrecisionPolicy numericalPrecisionPolicy() const noexcept;
        /*! Set the numerical precision policy for the current query. */
        void setNumericalPrecisionPolicy(NumericalPrecisionPolicy precision) noexcept;

        /*! Get the SQL database driver used to access the database connection (const). */
        const SqlDriver *driver() const noexcept;
        /*! Get the SQL database driver used to access the database connection (const). */
        std::weak_ptr<const SqlDriver> driverWeak() const noexcept;

        /* Normal queries */
        /*! Execute the given SQL query (non-prepared only). */
        bool exec(const QString &query);

        /* Prepared queries */
        /*! Prepares the given SQL query for execution. */
        bool prepare(const QString &query);
        /*! Execute a previously prepared SQL query. */
        bool exec();

        /*! Bound the positional placeholder value at the given index for the prepared
            statement. */
        void bindValue(size_type index, const QVariant &value,
                       ParamType /*unused*/ = ParamType::In);
        /*! Bound the positional placeholder value at the given index for the prepared
            statement. */
        void bindValue(size_type index, QVariant &&value,
                       ParamType /*unused*/ = ParamType::In);

        /*! Add the placeholder value to the list of positional bound values. */
        void addBindValue(const QVariant &value, ParamType /*unused*/ = ParamType::In);
        /*! Add the placeholder value to the list of positional bound values. */
        void addBindValue(QVariant &&value, ParamType /*unused*/ = ParamType::In);

        /*! Get the placeholder value at the given index position. */
        QVariant boundValue(size_type index) const;
        /*! Get a QVariant vector of all bound values. */
        QVariantList boundValues() const;

        /* Result sets */
        /*! Get a SqlRecord containing the field information for the current query. */
        SqlRecord record() const;
        /*! Get the ID of the most recent inserted row if the database supports it. */
        QVariant lastInsertId() const;

        /*! Retrieve the next record and position the cursor on it. */
        bool next();
        /*! Retrieve the previous record and position the cursor on it. */
        bool previous();
        /*! Retrieve the first record and position the cursor on it. */
        bool first();
        /*! Retrieve the last record and position the cursor on it. */
        bool last();
        /*! Retrieve the record at the given index and position the cursor on it. */
        bool seek(size_type index, bool relative = false);

        /*! Get the field value at the given index in the current record. */
        QVariant value(size_type index) const;
        /*! Get the field value with the field name in the current record. */
        QVariant value(const QString &name) const;

        /*! Determine whether the field at the given index is NULL. */
        bool isNull(size_type index) const;
        /*! Determine whether the field with the given field name is NULL. */
        bool isNull(const QString &name) const;

        /*! Get the size of the result (number of rows returned), -1 if the size can't be
            determined (database must support reporting about query sizes). */
        size_type size() const noexcept;
        /*! Get the number of affected rows for DML queries or -1 if the size can't be
            determined. */
        size_type numRowsAffected() const;

        /* Others */
        /*! Clear the result set and releases any resources held by the query. */
        void clear();
        /*! Instruct the database driver that no more data will be fetched from this query
            until it is re-executed. */
        void finish() noexcept;

    private:
        /* Getters / Setters */
        /*! Get the SQL database driver used to access the database connection. */
        std::weak_ptr<SqlDriver> driverWeakInternal() noexcept;

        /* Result sets */
        /*! Normal seek. */
        bool seekArbitrary(size_type index, size_type &actualIdx) noexcept;
        /*! Relative seek. */
        bool seekRelative(size_type index, size_type &actualIdx);
        /*! Map the given index to the fetch-related methods that are available. */
        bool mapSeekToFetch(size_type actualIdx);

        /* Constructors */
        /*! Initialize implementation-dependent query result set for the default
            connection. */
        static std::unique_ptr<SqlResult> initSqlResult();
        /*! Initialize implementation-dependent query result set for the given
            connection. */
        static std::unique_ptr<SqlResult> initSqlResult(const SqlDatabase &connection);

        /* Data members */
        /*! Query result set. */
        NotNull<std::unique_ptr<SqlResult>> m_sqlResult;
    };

    /* public */

    constexpr void SqlQuery::swap(SqlQuery &other) noexcept
    {
        std::swap(m_sqlResult, other.m_sqlResult);
    }

} // namespace Orm::Drivers

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_DRIVERS_SQLQUERY_HPP
