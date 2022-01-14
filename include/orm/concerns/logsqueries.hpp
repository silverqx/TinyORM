#pragma once
#ifndef ORM_CONCERNS_LOGSQUERIES_HPP
#define ORM_CONCERNS_LOGSQUERIES_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include <QtSql/QSqlQuery>

#include "orm/macros/export.hpp"
#include "orm/types/log.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm
{

class DatabaseConnection;

namespace Concerns
{

    /*! Logs executed queries to the console. */
    class SHAREDLIB_EXPORT LogsQueries
    {
        Q_DISABLE_COPY(LogsQueries)

    public:
        /*! Default constructor. */
        inline LogsQueries() = default;
        /*! Virtual destructor, to pass -Weffc++. */
        inline virtual ~LogsQueries() = default;

        /*! Log a query into the connection's query log. */
        void logQuery(const QSqlQuery &query, std::optional<qint64> elapsed) const;
        /*! Log a query into the connection's query log. */
        void logQuery(const std::tuple<int, QSqlQuery> &queryResult,
                      std::optional<qint64> elapsed) const;
        /*! Log a query into the connection's query log in the pretending mode. */
        void logQueryForPretend(const QString &query,
                                const QVector<QVariant> &bindings) const;
        /*! Log a transaction query into the connection's query log. */
        void logTransactionQuery(const QString &query,
                                 std::optional<qint64> elapsed) const;
        /*! Log a transaction query into the connection's query log
            in the pretending mode. */
        void logTransactionQueryForPretend(const QString &query) const;

        /*! Get the connection query log. */
        inline std::shared_ptr<QVector<Log>> getQueryLog() const;
        /*! Clear the query log. */
        void flushQueryLog();
        /*! Enable the query log on the connection. */
        void enableQueryLog();
        /*! Disable the query log on the connection. */
        inline void disableQueryLog();
        /*! Determine whether we're logging queries. */
        inline bool logging() const;
        /*! The current order value for a query log record. */
        inline static std::size_t getQueryLogOrder();

    protected:
        /*! Execute the given callback in "dry run" mode. */
        QVector<Log>
        withFreshQueryLog(const std::function<QVector<Log>()> &callback);

        /*! Indicates if changes have been made to the database. */
        bool m_recordsModified = false;
        /*! All of the queries run against the connection. */
        std::shared_ptr<QVector<Log>> m_queryLog = nullptr;
        /*! ID of the query log record. */
        inline static std::atomic<std::size_t> m_queryLogId = 0;

    private:
        /*! Dynamic cast *this to the DatabaseConnection & derived type. */
        const DatabaseConnection &databaseConnection() const;

        /*! Indicates whether queries are being logged (private intentionally). */
        bool m_loggingQueries = false;
        /*! All of the queries run against the connection. */
        std::shared_ptr<QVector<Log>> m_queryLogForPretend = nullptr;
    };

    /* public */

    std::shared_ptr<QVector<Log>> LogsQueries::getQueryLog() const
    {
        return m_queryLog;
    }

    void LogsQueries::disableQueryLog()
    {
        m_loggingQueries = false;
    }

    bool LogsQueries::logging() const
    {
        return m_loggingQueries;
    }

    std::size_t LogsQueries::getQueryLogOrder()
    {
        return m_queryLogId;
    }

} // namespace Concerns
} // namespace Orm

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_CONCERNS_LOGSQUERIES_HPP
