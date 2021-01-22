#ifndef CONNECTIONINTERFACE_HPP
#define CONNECTIONINTERFACE_HPP

#include <QtSql/QSqlQuery>

#include "orm/query/expression.hpp"

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm
{

namespace Query
{
    class Builder;
}
    using QueryBuilder = Query::Builder;

    class ConnectionInterface
    {
    public:
        inline virtual ~ConnectionInterface() = default;

        /*! Begin a fluent query against a database table. */
        virtual QSharedPointer<QueryBuilder>
        table(const QString &table, const QString &as = "") = 0;

        /*! Get a new query builder instance. */
        virtual QSharedPointer<QueryBuilder> query() = 0;

        /*! Get a new raw query expression. */
        virtual Query::Expression raw(const QVariant &value) const = 0;

        /*! Execute a Closure within a transaction. */
//        public function transaction(Closure $callback, $attempts = 1);

        /*! Start a new database transaction. */
        virtual bool beginTransaction() = 0;

        /*! Commit the active database transaction. */
        virtual bool commit() = 0;

        /*! Rollback the active database transaction. */
        virtual bool rollBack() = 0;

        /*! Get the number of active transactions. */
        virtual uint transactionLevel() const = 0;

        /*! Run a select statement and return a single result. */
        virtual std::tuple<bool, QSqlQuery>
        selectOne(const QString &queryString,
                  const QVector<QVariant> &bindings = {}) = 0;

        /*! Run a select statement against the database. */
        virtual std::tuple<bool, QSqlQuery>
        select(const QString &queryString,
               const QVector<QVariant> &bindings = {}) = 0;

        /*! Run a select statement against the database and returns a generator. */
//        public function cursor($query, $bindings = [], $useReadPdo = true);

        /*! Run an insert statement against the database. */
        virtual std::tuple<bool, QSqlQuery>
        insert(const QString &queryString,
               const QVector<QVariant> &bindings = {}) = 0;

        /*! Run an update statement against the database. */
        virtual std::tuple<int, QSqlQuery>
        update(const QString &queryString,
               const QVector<QVariant> &bindings = {}) = 0;

        /*! Run a delete statement against the database. */
        virtual std::tuple<int, QSqlQuery>
        remove(const QString &queryString,
               const QVector<QVariant> &bindings = {}) = 0;

        /*! Execute an SQL statement and return the boolean result. */
        virtual std::tuple<bool, QSqlQuery>
        statement(const QString &queryString,
                  const QVector<QVariant> &bindings = {}) = 0;

        /*! Run an SQL statement and get the number of rows affected. */
        virtual std::tuple<int, QSqlQuery>
        affectingStatement(const QString &queryString,
                           const QVector<QVariant> &bindings = {}) = 0;

        /*! Get a new QSqlQuery instance for the current connection. */
        virtual QSqlQuery getQtQuery() = 0;

        /*! Prepare the query bindings for execution. */
        virtual QVector<QVariant>
        prepareBindings(const QVector<QVariant> &bindings) const = 0;

        /*! Check database connection and show warnings when the state changed. */
        virtual bool pingDatabase() = 0;

        /*! Execute the given callback in "dry run" mode. */
//        public function pretend(Closure $callback);

        /*! Get the database connection name. */
        virtual const QString getName() const = 0;

        /*! Get the name of the connected database. */
        virtual const QString &getDatabaseName() const = 0;
    };

} // namespace Orm
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif

#endif // CONNECTIONINTERFACE_HPP
