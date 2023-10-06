#include "orm/concerns/logsqueries.hpp"

#ifdef TINYORM_DEBUG_SQL
#  include <QDebug>
#endif

#include "orm/databaseconnection.hpp"
#include "orm/macros/likely.hpp"
#ifdef TINYORM_DEBUG_SQL
#  include "orm/utils/query.hpp"
#endif

TINYORM_BEGIN_COMMON_NAMESPACE

#ifdef TINYORM_DEBUG_SQL
using QueryUtils = Orm::Utils::Query;
#endif

namespace Orm::Concerns
{

// We don't need the Orm::SqlQuery overloads as all bindings are already prepared

/* public */

void LogsQueries::logQueryForPretend(
        const QString &query, const QVector<QVariant> &preparedBindings,
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

    qDebug("Pretended %s query (%s) : %s",
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

    qDebug("Executed transaction query (%llims%s) : %s",
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

    qDebug("Pretended transaction query (%s) : %s",
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
        m_queryLog = std::make_shared<QVector<Log>>();

    m_loggingQueries = true;
}

/* protected */

QVector<Log>
LogsQueries::withFreshQueryLog(const std::function<QVector<Log>()> &callback)
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
        m_queryLogForPretend = std::make_shared<QVector<Log>>();

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
        const QSqlQuery &query, const std::optional<qint64> elapsed,
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
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
                            query.boundValues(),
#else
                            convertNamedToPositionalBindings(query.boundValues()),
#endif
                            Log::Type::NORMAL, ++m_queryLogId,
                            elapsed ? *elapsed : -1, query.size(),
                            query.numRowsAffected()});
    }

#ifdef TINYORM_DEBUG_SQL
    // Debugging SQL queries is disabled
    if (!m_debugSql)
        return;

    const auto &connectionName = databaseConnection().getName();

    qDebug("Executed %s query (%llims, %i results, %i affected%s) : %s",
           type.toUtf8().constData(),
           elapsed ? *elapsed : -1,
           query.size(),
           query.numRowsAffected(),
           connectionName.isEmpty() ? ""
                                    : QStringLiteral(", %1").arg(connectionName)
                                      .toUtf8().constData(),
           QueryUtils::parseExecutedQuery(query).toUtf8().constData());
#endif
}

QVector<QVariant>
LogsQueries::convertNamedToPositionalBindings(QVariantMap &&bindings) // NOLINT(cppcoreguidelines-rvalue-reference-param-not-moved)
{
    QVector<QVariant> result;
    result.reserve(bindings.size());

    for (auto &&binding : bindings)
        result << std::move(binding);

    return result;
}

const DatabaseConnection &LogsQueries::databaseConnection() const
{
    return dynamic_cast<const DatabaseConnection &>(*this);
}

} // namespace Orm::Concerns

TINYORM_END_COMMON_NAMESPACE
