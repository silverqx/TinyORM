#include "orm/concerns/logsqueries.hpp"

#ifdef TINYORM_DEBUG_SQL
#  include <QDebug>
#endif

#include "orm/databaseconnection.hpp"
#include "orm/macros/likely.hpp"
#include "orm/utils/query.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

using QueryUtils = Orm::Utils::Query;

namespace Orm::Concerns
{

// We don't need the Orm::SqlQuery overloads as all bindings are already prepared

/* public */

void LogsQueries::logQueryForPretend(
        const QString &query, const QList<QVariant> &preparedBindings,
#ifdef TINYORM_DEBUG_SQL
        const QString &type) const
#else
        const QString &/*unused*/) const
#endif
{
    if (m_loggingQueries && m_queryLog)
        m_queryLog->append({query, preparedBindings, Log::Type::NORMAL, ++m_queryLogId});

#ifdef TINYORM_DEBUG_SQL
    // Debugging SQL queries is disabled
    if (!m_debugSql)
        return;

    const auto &connectionName = databaseConnection().getName();

    qDebug("Pretended %s query (%s) : %s", // NOLINT(cppcoreguidelines-pro-type-vararg)
           type.toUtf8().constData(),
           connectionName.isEmpty() ? "" : connectionName.toUtf8().constData(),
           QueryUtils::parseExecutedQueryForPretend(query, preparedBindings)
           .toUtf8().constData());
#endif
}

void LogsQueries::logTransactionQuery(
        const QString &query, const std::optional<qint64> elapsed) const
{
    if (m_loggingQueries && m_queryLog)
        m_queryLog->append({query, {}, Log::Type::TRANSACTION, ++m_queryLogId,
                            elapsed ? *elapsed : -1});

#ifdef TINYORM_DEBUG_SQL
    // Debugging SQL queries is disabled
    if (!m_debugSql)
        return;

    const auto &connectionName = databaseConnection().getName();

    qDebug("Executed transaction query (%llims%s) : %s", // NOLINT(cppcoreguidelines-pro-type-vararg)
           elapsed ? *elapsed : -1,
           connectionName.isEmpty() ? ""
                                    : QStringLiteral(", %1").arg(connectionName)
                                      .toUtf8().constData(),
           query.toUtf8().constData());
#endif
}

void LogsQueries::logTransactionQueryForPretend(const QString &query) const
{
    if (m_loggingQueries && m_queryLog)
        m_queryLog->append({query, {}, Log::Type::TRANSACTION, ++m_queryLogId});

#ifdef TINYORM_DEBUG_SQL
    // Debugging SQL queries is disabled
    if (!m_debugSql)
        return;

    const auto &connectionName = databaseConnection().getName();

    qDebug("Pretended transaction query (%s) : %s", // NOLINT(cppcoreguidelines-pro-type-vararg)
           connectionName.isEmpty() ? "" : connectionName.toUtf8().constData(),
           query.toUtf8().constData());
#endif
}

void LogsQueries::flushQueryLog()
{
    // TODO sync silverqx
    if (m_queryLog)
        m_queryLog->clear();

    m_queryLogId = 0;
}

void LogsQueries::enableQueryLog()
{
    /* Instantiate the query log vector lazily, right before it is really needed,
       and do not flush it. */
    if (!m_queryLog)
        m_queryLog = std::make_shared<QList<Log>>();

    m_loggingQueries = true;
}

/* protected */

QList<Log>
LogsQueries::withFreshQueryLog(const std::function<QList<Log>()> &callback)
{
    /* First we will back up the value of the logging queries data members and then
       we'll enable query logging. The query log will also get cleared so we will
       have a new log of all the queries that will be executed. */
    const auto loggingQueries = m_loggingQueries;
    const auto queryLogId = m_queryLogId.load();
    m_queryLogId.store(0);

    enableQueryLog();

    if (m_queryLogForPretend) T_LIKELY
        m_queryLogForPretend->clear();
    else T_UNLIKELY
        // Create the query log lazily, right before it is really needed
        m_queryLogForPretend = std::make_shared<QList<Log>>();

    // Swap query logs, so I don't have to manage separate logic for pretend code
    m_queryLog.swap(m_queryLogForPretend);

    /* Now we'll execute this callback and capture the result. Once it has been
       executed we will restore original values and give back the value of the callback
       so the original callers can have the results. */
    auto result = std::invoke(callback);

    // Restore
    m_queryLog.swap(m_queryLogForPretend);
    m_loggingQueries = loggingQueries;
    m_queryLogId.store(queryLogId);

    // NRVO kicks in
    return result;
}

/* private */

void LogsQueries::logQueryInternal(
        TSqlQuery &query, const std::optional<qint64> elapsed,
#ifdef TINYORM_DEBUG_SQL
        const QString &type) const
#else
        const QString &/*unused*/) const
#endif
{
    if (m_loggingQueries && m_queryLog) {
        auto executedQuery = query.executedQuery();
        if (executedQuery.isEmpty())
            executedQuery = query.lastQuery();

        m_queryLog->append({std::move(executedQuery),
                            query.boundValues(),
                            Log::Type::NORMAL, ++m_queryLogId,
                            elapsed ? *elapsed : -1,
                            QueryUtils::queryResultSize(query),
                            query.isActive() ? query.numRowsAffected() : -1});
    }

#ifdef TINYORM_DEBUG_SQL
    // Debugging SQL queries is disabled
    if (!m_debugSql)
        return;

    const auto &connectionName = databaseConnection().getName();

    qDebug("Executed %s query (%llims, %i results, %i affected%s) : %s", // NOLINT(cppcoreguidelines-pro-type-vararg)
           type.toUtf8().constData(),
           elapsed ? *elapsed : -1,
           QueryUtils::queryResultSize(query),
           query.isActive() ? query.numRowsAffected() : -1,
           connectionName.isEmpty() ? ""
                                    : QStringLiteral(", %1").arg(connectionName)
                                      .toUtf8().constData(),
           QueryUtils::parseExecutedQuery(query).toUtf8().constData());
#endif
}

const DatabaseConnection &LogsQueries::databaseConnection() const
{
    return dynamic_cast<const DatabaseConnection &>(*this);
}

} // namespace Orm::Concerns

TINYORM_END_COMMON_NAMESPACE
