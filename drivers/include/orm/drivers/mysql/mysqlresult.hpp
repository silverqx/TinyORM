#pragma once
#ifndef ORM_DRIVERS_MYSQL_MYSQLRESULT_HPP
#define ORM_DRIVERS_MYSQL_MYSQLRESULT_HPP

#include <orm/macros/systemheader.hpp>
TINY_SYSTEM_HEADER

#include "orm/drivers/sqlresult.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Drivers::MySql
{

    class MySqlResultPrivate;

    class SHAREDLIB_EXPORT MySqlResult final : public SqlResult
    {
        Q_DISABLE_COPY_MOVE(MySqlResult)
        Q_DECLARE_PRIVATE(MySqlResult)

        friend class MySqlDriver;

    public:
        /*! Constructor. */
        explicit MySqlResult(const MySqlDriver *driver);
        /*! Virtual destructor. */
        ~MySqlResult() final;

        /*! Returns the low-level database result set handle (MYSQL_RES or MYSQL_STMT). */
        QVariant handle() const final;

    protected:
        /* Normal queries */
        bool exec(const QString &query) final;

        /* Prepared queries */
        bool prepare(const QString &query) final;
        bool exec() final;

        /* Result */
        SqlRecord record() const final;
        QVariant lastInsertId() const final;

        bool fetchNext() final;
        bool fetch(int i) final;
        bool fetchFirst() final;
        bool fetchLast() final;

        QVariant data(int field) final;
        bool isNull(int field) final;

        int size() final;
        int numRowsAffected() final;

        void detachFromResultSet() final;

        /* Others */
        void cleanup();

    private:
        void mysqlFreeResults() const;
        void mysqlStmtClose();
    };

} // namespace Orm::Drivers::MySql

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_DRIVERS_MYSQL_MYSQLRESULT_HPP
