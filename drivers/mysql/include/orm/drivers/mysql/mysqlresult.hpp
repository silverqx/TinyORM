#pragma once
#ifndef ORM_DRIVERS_MYSQL_MYSQLRESULT_HPP
#define ORM_DRIVERS_MYSQL_MYSQLRESULT_HPP

#include <orm/macros/systemheader.hpp>
TINY_SYSTEM_HEADER

#include "orm/drivers/sqlresult.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Drivers::MySql
{

    class MySqlDriver;
    class MySqlResultPrivate;

    /*! MySQL database result set. */
    class TINYDRIVERS_EXPORT MySqlResult final : public SqlResult
    {
        Q_DISABLE_COPY_MOVE(MySqlResult)
        Q_DECLARE_PRIVATE(MySqlResult)

    public:
        /*! Constructor. */
        explicit MySqlResult(const std::weak_ptr<MySqlDriver> &driver);
        /*! Virtual destructor. */
        ~MySqlResult() noexcept final;

        /*! Returns the low-level database result set handle (MYSQL_RES or MYSQL_STMT). */
        QVariant handle() const noexcept final;

    protected:
        /* Normal queries */
        /*! Execute the given SQL query (non-prepared only). */
        bool exec(const QString &query) final;

        /* Prepared queries */
        /*! Prepares the given SQL query for execution. */
        bool prepare(const QString &query) final;
        /*! Execute a previously prepared SQL query. */
        bool exec() final;

        /* Result sets */
        /*! Get a SqlRecord containing the field information for the current query. */
        SqlRecord record() const final;
        /*! Get the ID of the most recent inserted row if the database supports it. */
        QVariant lastInsertId() const final;

        /*! Retrieve the record at the given index and position the cursor on it. */
        bool fetch(size_type index) final;
        /*! Retrieve the first record and position the cursor on it. */
        bool fetchFirst() final;
        /*! Retrieve the last record and position the cursor on it. */
        bool fetchLast() final;
        /*! Retrieve the next record and position the cursor on it. */
        bool fetchNext() final;

        /*! Get the field value at the given index in the current record. */
        QVariant data(size_type index) final;
        /*! Determine whether the field at the given index is NULL. */
        bool isNull(size_type index) final;

        /*! Get the size of the result (number of rows returned), -1 if the size can't be
            determined (database must support reporting about query sizes). */
        size_type size() final;
        /*! Get the number of affected rows for DML queries or -1 if the size can't be
            determined. */
        size_type numRowsAffected() final;

        /*! Releases memory associated with the current result set. */
        void detachFromResultSet() final;

        /* Cleanup */
        /*! Main cleanup method, free prepared and non-prepared statements. */
        void cleanup();

    private:
        /* Result sets */
        /*! Fetch the next row in the result set. */
        bool mysqlStmtFetch();

        /* Cleanup */
        /*! Main cleanup method, free prepared and non-prepared statements, noexcept. */
        void cleanupForDtor() noexcept;

        /*! Free the memory allocated for result sets. */
        void mysqlFreeResults();
        /*! Free the memory allocated for result sets, noexcept version. */
        void mysqlFreeResultsForDtor() noexcept;
        /*! Free the memory allocated for multi-result sets, noexcept version. */
        void mysqlFreeMultiResultsForDtor() noexcept;
        /*! Close the prepared statement and deallocate the statement handler. */
        void mysqlStmtClose();

        /* Others */
        /*! Throw an exception if an index for result fields vector is out of bounds. */
        void throwIfBadResultFieldsIndex(std::size_t index) const;
    };

} // namespace Orm::Drivers::MySql

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_DRIVERS_MYSQL_MYSQLRESULT_HPP
