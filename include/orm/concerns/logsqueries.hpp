#pragma once
#ifndef ORM_CONCERNS_LOGSQUERIES_HPP
#define ORM_CONCERNS_LOGSQUERIES_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include "orm/macros/sqldrivermappings.hpp"
#include TINY_INCLUDE_TSqlQuery

#include <memory>
#include <optional>

#include "orm/config.hpp" // IWYU pragma: keep

#include "orm/macros/export.hpp"
#include "orm/types/log.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm
{

class DatabaseConnection;

namespace Concerns
{

    /*! Logs executed queries to the console. */
    class TINYORM_EXPORT LogsQueries
    {
        Q_DISABLE_COPY_MOVE(LogsQueries)

    public:
        /*! Default constructor. */
        LogsQueries() = default;
        /*! Pure virtual destructor, to pass -Weffc++. */
        inline virtual ~LogsQueries() = 0;

        /*! Log a query into the connection's query log. */
        inline void logQuery(TSqlQuery &query, std::optional<qint64> elapsed,
                             const QString &type) const;
        /*! Log a query into the connection's query log. */
        inline void logQuery(std::tuple<int, TSqlQuery> &queryResult,
                             std::optional<qint64> elapsed, const QString &type) const;
        /*! Log a query into the connection's query log in the pretending mode. */
        void logQueryForPretend(const QString &query,
                                const QList<QVariant> &preparedBindings,
                                const QString &type) const;
        /*! Log a transaction query into the connection's query log. */
        void logTransactionQuery(const QString &query,
                                 std::optional<qint64> elapsed) const;
        /*! Log a transaction query into the connection's query log
            in the pretending mode. */
        void logTransactionQueryForPretend(const QString &query) const;

        /*! Get the connection query log. */
        inline std::shared_ptr<QList<Log>> getQueryLog() const noexcept;
        /*! Clear the query log. */
        void flushQueryLog();
        /*! Enable the query log on the connection. */
        void enableQueryLog();
        /*! Disable the query log on the connection. */
        inline void disableQueryLog() noexcept;
        /*! Determine whether we're logging queries. */
        inline bool logging() const noexcept;
        /*! The current order value for a query log record. */
        inline static std::size_t getQueryLogOrder() noexcept;

        /*! Determine whether debugging SQL queries is enabled/disabled (logging
            to the console using qDebug()). */
        inline bool debugSql() const noexcept;
        /*! Disable debugging SQL queries (logging to the console using qDebug()). */
        inline void disableDebugSql() noexcept;
        /*! Enable debugging SQL queries (logging to the console using qDebug()). */
        inline void enableDebugSql() noexcept;

    protected:
        /*! Execute the given callback in "dry run" mode. */
        QList<Log>
        withFreshQueryLog(const std::function<QList<Log>()> &callback);

        /*! Indicates if changes have been made to the database. */
        bool m_recordsModified = false;
        /*! All of the queries run against the connection. */
        std::shared_ptr<QList<Log>> m_queryLog = nullptr;
        /*! ID of the query log record. */
        inline static std::atomic<std::size_t> m_queryLogId = 0;

#ifdef TINYORM_DEBUG_SQL
        /*! Indicates whether logging of sql queries is enabled. */
        bool m_debugSql = true;
#else
        /*! Indicates whether logging of sql queries is enabled. */
        bool m_debugSql = false;
#endif

    private:
        /*! Log a query into the connection's query log. */
        void logQueryInternal(TSqlQuery &query, std::optional<qint64> elapsed,
                              const QString &type) const;

        /*! Dynamic cast *this to the DatabaseConnection & derived type. */
        const DatabaseConnection &databaseConnection() const;

        /*! Indicates whether queries are being logged (private intentionally). */
        bool m_loggingQueries = false;
        /*! All of the queries run against the connection. */
        std::shared_ptr<QList<Log>> m_queryLogForPretend = nullptr;
    };

    /* public */

    LogsQueries::~LogsQueries() = default;

    void LogsQueries::logQuery(
            TSqlQuery &queryResult, std::optional<qint64> elapsed,
            const QString &type) const
    {
        logQueryInternal(queryResult, elapsed, type);
    }

    void LogsQueries::logQuery(
            std::tuple<int, TSqlQuery> &queryResult,
            std::optional<qint64> elapsed, const QString &type) const
    {
        logQueryInternal(std::get<1>(queryResult), elapsed, type);
    }

    std::shared_ptr<QList<Log>> LogsQueries::getQueryLog() const noexcept
    {
        return m_queryLog;
    }

    void LogsQueries::disableQueryLog() noexcept
    {
        m_loggingQueries = false;
    }

    bool LogsQueries::logging() const noexcept
    {
        return m_loggingQueries;
    }

    std::size_t LogsQueries::getQueryLogOrder() noexcept
    {
        return m_queryLogId;
    }

    bool LogsQueries::debugSql() const noexcept
    {
        return m_debugSql;
    }

    void LogsQueries::disableDebugSql() noexcept
    {
        m_debugSql = false;
    }

    void LogsQueries::enableDebugSql() noexcept
    {
        m_debugSql = true;
    }

} // namespace Concerns
} // namespace Orm

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_CONCERNS_LOGSQUERIES_HPP
