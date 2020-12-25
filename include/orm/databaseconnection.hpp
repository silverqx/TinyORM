#ifndef DATABASECONNECTION_H
#define DATABASECONNECTION_H

#include <QtSql/QSqlDatabase>

#include "orm/grammar.hpp"
#include "orm/query/querybuilder.hpp"

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm
{

    class SHAREDLIB_EXPORT DatabaseConnection final
    {
        Q_DISABLE_COPY(DatabaseConnection)

    public:
        static DatabaseConnection &instance();
        static void freeInstance();

        /*! Return underlying database connection. */
        static QSqlDatabase database();

        QSqlQuery query() const;
        QSharedPointer<QueryBuilder> queryBuilder() const;
        /*! Check database connection and show warnings when the state changed. */
        bool pingDatabase();
        bool transaction();
        bool commit();
        bool rollback();
        bool savepoint(const QString &id);
        bool rollbackToSavepoint(const QString &id);

        /*! Run a select statement against the database. */
        std::tuple<bool, QSqlQuery>
        select(const QString &queryString,
               const QVector<QVariant> &bindings = {}) const;
        /*! Run an insert statement against the database. */
        std::tuple<bool, QSqlQuery>
        insert(const QString &queryString,
               const QVector<QVariant> &bindings = {}) const;
        /*! Run an update statement against the database. */
        std::tuple<int, QSqlQuery>
        update(const QString &queryString,
               const QVector<QVariant> &bindings = {}) const;
        /*! Run a delete statement against the database. */
        std::tuple<int, QSqlQuery>
        remove(const QString &queryString,
               const QVector<QVariant> &bindings = {}) const;

        /*! Execute an SQL statement and return the boolean result and QSqlQuery. */
        std::tuple<bool, QSqlQuery>
        statement(const QString &queryString,
                  const QVector<QVariant> &bindings = {}) const;
        /*! Run an SQL statement and get the number of rows affected. */
        std::tuple<int, QSqlQuery>
        affectingStatement(const QString &queryString,
                           const QVector<QVariant> &bindings = {}) const;

        /*! Get a new raw query expression. */
        inline Expression raw(const QVariant &value) const
        { return value; }

        /*! Get the database connection name. */
        inline const QString getName() const
        { return CONNECTION_NAME; }

    private:
        static DatabaseConnection *m_instance;

        DatabaseConnection();
        ~DatabaseConnection() = default;

        static const char *CONNECTION_NAME;
        static const char *SAVEPOINT_NAMESPACE;

        void showDbDisconnected();
        void showDbConnected();

        /*! Prepare an SQL statement and return the query object. */
        QSqlQuery prepareQuery(const QString &queryString,
                               const QVector<QVariant> &bindings = {}) const;

        bool m_dbDisconnectedShowed = false;
        bool m_dbConnectedShowed = false;
        /*! The connection is in the transaction state. */
        bool m_inTransaction = false;
        /*! Active savepoints counter. */
        uint m_savepoints = 0;
        Grammar m_grammar;
    };

} // namespace Orm
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif

#endif // DATABASECONNECTION_H
