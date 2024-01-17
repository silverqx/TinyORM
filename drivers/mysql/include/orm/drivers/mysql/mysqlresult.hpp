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
        ~MySqlResult() final;

        /*! Returns the low-level database result set handle (MYSQL_RES or MYSQL_STMT). */
        QVariant handle() const final;

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
        bool fetch(int i) final;
        /*! Retrieve the first record and position the cursor on it. */
        bool fetchFirst() final;
        /*! Retrieve the last record and position the cursor on it. */
        bool fetchLast() final;
        /*! Retrieve the next record and position the cursor on it. */
        bool fetchNext() final;

        /*! Get the field value at the given index in the current record. */
        QVariant data(int field) final;
        /*! Determine whether the field at the given index is NULL. */
        bool isNull(int field) final;

        /*! Get the size of the result (number of rows returned), -1 if the size can't be
            determined (database must support reporting about query sizes). */
        int size() final;
        /*! Get the number of affected rows for DML queries or -1 if the size can't be
            determined. */
        int numRowsAffected() final;

        /*! Releases memory associated with the current result set. */
        void detachFromResultSet() final;

        /* Others */
        /*! Main cleanup method, free prepared and non-prepared statements. */
        void cleanup(bool fromDestructor = false);

    private:
        /*! Free the memory allocated for result sets. */
        void mysqlFreeResults();
        /*! Close the prepared statement and deallocate the statement handler. */
        void mysqlStmtClose();

        /*! Throw an exception if an index for result fields vector is out of bounds. */
        void throwIfBadResultFieldsIndex(std::size_t index) const;
    };

} // namespace Orm::Drivers::MySql

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_DRIVERS_MYSQL_MYSQLRESULT_HPP
