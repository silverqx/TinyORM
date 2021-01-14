#ifndef DATABASECONNECTION_H
#define DATABASECONNECTION_H

#include <QtSql/QSqlDatabase>

#include "orm/connectors/connectorinterface.hpp"
#include "orm/grammar.hpp"
#include "orm/query/querybuilder.hpp"

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm
{

    // TODO now SHAREDLIB_EXPORT and Q_DISABLE_COPY silverqx
    class SHAREDLIB_EXPORT DatabaseConnection
    {
    public:
        inline static const char *defaultConnectionName {const_cast<char *>("tinyorm_default")};

        explicit DatabaseConnection(
                const std::function<Connectors::ConnectionName()> &connection,
                const QString &database = "", const QString tablePrefix = "",
                const QVariantHash &config = {});

        /*! Begin a fluent query against a database table. */
        QSharedPointer<QueryBuilder>
        table(const QString &table, const QString &as = "");

        /*! Get a new query builder instance. */
        QSharedPointer<QueryBuilder> query();

        /*! Get a new raw query expression. */
        inline Query::Expression raw(const QVariant &value) const
        { return value; }

        /*! Check database connection and show warnings when the state changed. */
        bool pingDatabase();
        /*! Start a new database transaction. */
        bool transaction();
        /*! Commit the active database transaction. */
        bool commit();
        /*! Rollback the active database transaction. */
        bool rollback();
        /*! Start a new named transaction savepoint. */
        bool savepoint(const QString &id);
        /*! Rollback to a named transaction savepoint. */
        bool rollbackToSavepoint(const QString &id);

        /*! Run a select statement against the database. */
        std::tuple<bool, QSqlQuery>
        select(const QString &queryString,
               const QVector<QVariant> &bindings = {});
        /*! Run a select statement and return a single result. */
        std::tuple<bool, QSqlQuery>
        selectOne(const QString &queryString,
                  const QVector<QVariant> &bindings = {});
        /*! Run an insert statement against the database. */
        std::tuple<bool, QSqlQuery>
        insert(const QString &queryString,
               const QVector<QVariant> &bindings = {});
        /*! Run an update statement against the database. */
        std::tuple<int, QSqlQuery>
        update(const QString &queryString,
               const QVector<QVariant> &bindings = {});
        /*! Run a delete statement against the database. */
        std::tuple<int, QSqlQuery>
        remove(const QString &queryString,
               const QVector<QVariant> &bindings = {});

        /*! Execute an SQL statement and return the boolean result and QSqlQuery. */
        std::tuple<bool, QSqlQuery>
        statement(const QString &queryString,
                  const QVector<QVariant> &bindings = {});
        /*! Run an SQL statement and get the number of rows affected. */
        std::tuple<int, QSqlQuery>
        affectingStatement(const QString &queryString,
                           const QVector<QVariant> &bindings = {});

        /*! Get underlying database connection. */
        QSqlDatabase getQtConnection();
        /*! Get underlying database connection without executing any reconnect logic. */
        QSqlDatabase getRawQtConnection() const;

        /*! Get a new QSqlQuery instance for the current connection. */
        QSqlQuery getQtQuery();

        /*! Get the database connection name. */
        inline const QString getName() const
        { return getConfig("name").toString(); }

        /*! Get the name of the connected database. */
        inline const QString &getDatabaseName() const
        { return m_database; }

        /*! Set the reconnect instance on the connection. */
//        DatabaseConnection &setReconnector(callable $reconnector);

        /*! Get an option from the configuration options. */
        QVariant getConfig(const QString &option) const;
        /*! Get the configuration for the current connection. */
        QVariant getConfig() const;

    protected:
        /*! The active QSqlDatabase connection name. */
        std::optional<Connectors::ConnectionName> m_qtConnection;
        /*! The QSqlDatabase connection resolver. */
        const std::function<Connectors::ConnectionName()> m_qtConnectionResolver;
        /*! The name of the connected database. */
        const QString m_database;
        /*! The table prefix for the connection. */
        const QString m_tablePrefix {""};
        /*! The database connection configuration options. */
        const QVariantHash m_config;

    private:
        // TODO docs silverqx
        static const char *SAVEPOINT_NAMESPACE;

        // TODO docs silverqx
        void showDbDisconnected();
        void showDbConnected();

        /*! Prepare an SQL statement and return the query object. */
        QSqlQuery prepareQuery(const QString &queryString,
                               const QVector<QVariant> &bindings = {});

        // TODO docs silverqx
        bool m_dbDisconnectedShowed = false;
        bool m_dbConnectedShowed = false;
        /*! The connection is in the transaction state. */
        bool m_inTransaction = false;
        /*! Active savepoints counter. */
        uint m_savepoints = 0;
        /*! The query grammar implementation. */
        Grammar m_queryGrammar;
    };

} // namespace Orm
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif

#endif // DATABASECONNECTION_H
