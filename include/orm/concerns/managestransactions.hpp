#pragma once
#ifndef ORM_CONCERNS_MANAGESTRANSACTIONS_HPP
#define ORM_CONCERNS_MANAGESTRANSACTIONS_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include <QString>

#include "orm/macros/commonnamespace.hpp"
#include "orm/macros/export.hpp"

#ifdef TINYORM_USING_QTSQLDRIVERS
class QSqlError;
class QSqlQuery;
#endif

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm
{

class DatabaseConnection;
class MySqlConnection;

namespace Concerns
{

    class CountsQueries;

    // TODO next transaction method with callback silverqx
    // TODO rewrite transactions, look at beginTransaction(), commit(), ... whats up, you will see immediately 😎 silverqx
    /*! Manages database transactions. */
    class TINYORM_EXPORT ManagesTransactions
    {
        Q_DISABLE_COPY_MOVE(ManagesTransactions)

        // To access resetTransactions() method
        friend DatabaseConnection;
        // To access resetTransactions() method
        friend MySqlConnection;

    public:
        /*! Default constructor. */
        ManagesTransactions();
        /*! Pure virtual destructor, to pass -Weffc++. */
        inline virtual ~ManagesTransactions() = 0;

        /*! Start a new database transaction. */
        bool beginTransaction();
        /*! Commit the active database transaction. */
        bool commit();
        /*! Rollback the active database transaction. */
        bool rollBack();
        /*! Start a new named transaction savepoint. */
        bool savepoint(const QString &id);
        /*! Start a new named transaction savepoint. */
        inline bool savepoint(std::size_t id);
        /*! Rollback to a named transaction savepoint. */
        bool rollbackToSavepoint(const QString &id);
        /*! Rollback to a named transaction savepoint. */
        inline bool rollbackToSavepoint(std::size_t id);
        /*! Get the number of active transactions. */
        inline std::size_t transactionLevel() const;

        /*! Determine whether the database connection is in the transaction state. */
        inline bool inTransaction() const;

        /*! Get namespace prefix for MySQL savepoints. */
        inline const QString &getSavepointNamespace() const;
        /*! Set namespace prefix for MySQL savepoints. */
        DatabaseConnection &setSavepointNamespace(const QString &savepointNamespace);

    private:
#ifdef TINYORM_USING_QTSQLDRIVERS
        /*! Start a new database transaction on the given connection. */
        static void runBeginTransaction(DatabaseConnection &connection);
        /*! Commit the active database transaction on the given connection. */
        static void runCommit(DatabaseConnection &connection);
        /*! Rollback the active database transaction on the given connection. */
        static void runRollBack(DatabaseConnection &connection);

        /*! Start a new or rollback to a named transaction savepoint. */
        static void runCommonSavepointQuery(
                    DatabaseConnection &connection, const QString &queryString,
                    const QString &functionName);

        /*! Transform QtSql transaction error (QSqlError) to TinyORM SqlTransactionError
            exception. */
        [[noreturn]] static void
        throwSqlTransactionError(const QString &functionName, const QString &queryString,
                                 QSqlError &&error);
        /*! Transform QtSql transaction error (QSqlError) to TinyORM SqlTransactionError
            exception (shortcut method). */
        [[noreturn]] static void
        throwSqlTransactionError(const QString &functionName, const QSqlQuery &sqlQuery);
#endif

        /*! Reset in transaction state and savepoints. */
        DatabaseConnection &resetTransactions();

        /*! Dynamic cast *this to the DatabaseConnection & derived type. */
        DatabaseConnection &databaseConnection();
        /*! Dynamic cast *this to the Concerns::CountsQueries & base type. */
        Concerns::CountsQueries &countsQueries();

        /*! Handle an exception that occurred when starting a transaction. */
        static void
        tryAgainIfCausedByLostConnectionStart(
                    DatabaseConnection &connection, const std::exception_ptr &ePtr,
                    const QString &errorMessage);
        /*! Handle an exception that occurred during the transaction commit, rollBack,
            savepoint, or rollbackToSavepoint. */
        void tryAgainIfCausedByLostConnectionCommon(
                    const std::exception_ptr &ePtr, const QString &errorMessage);

        /*! The connection is in the transaction state. */
        bool m_inTransaction = false;
        /*! Active savepoints counter. */
        std::size_t m_savepoints = 0;

        /*! Namespace prefix for MySQL savepoints. */
        QString m_savepointNamespace;
    };

    /* public */

    ManagesTransactions::~ManagesTransactions() = default;

    bool ManagesTransactions::savepoint(const std::size_t id)
    {
        return savepoint(QString::number(id));
    }

    bool ManagesTransactions::rollbackToSavepoint(const std::size_t id)
    {
        return rollbackToSavepoint(QString::number(id));
    }

    std::size_t ManagesTransactions::transactionLevel() const
    {
        return m_savepoints;
    }

    bool ManagesTransactions::inTransaction() const
    {
        return m_inTransaction;
    }

    const QString &ManagesTransactions::getSavepointNamespace() const
    {
        return m_savepointNamespace;
    }

} // namespace Concerns
} // namespace Orm

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_CONCERNS_MANAGESTRANSACTIONS_HPP
