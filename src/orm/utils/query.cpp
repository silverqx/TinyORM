#include "orm/utils/query.hpp"

#include <QDebug>
#include TINY_INCLUDE_TSqlDriver
#include TINY_INCLUDE_TSqlQuery

#include "orm/exceptions/invalidargumenterror.hpp"
#include "orm/utils/type.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Utils
{

/* We don't need the Orm::SqlQuery overloads for the parseExecutedQuery() and
   logExecutedQuery() as all bindings are already prepared. */

QString Query::parseExecutedQuery(const TSqlQuery &query)
{
    auto executedQuery = query.executedQuery();
    if (executedQuery.isEmpty())
        executedQuery = query.lastQuery();

    return replaceBindingsInSql(std::move(executedQuery), query.boundValues()).first;
}

#ifndef TINYORM_NO_DEBUG
void Query::logExecutedQuery(const TSqlQuery &query)
{
    qDebug().noquote() << QStringLiteral("Executed Query :")
                       << Query::parseExecutedQuery(query);
}
#else
void Query::logExecutedQuery(const TSqlQuery &/*unused*/)
{}
#endif

QVector<QVariantMap>
Query::zipForInsert(const QVector<QString> &columns,
                    const QVector<QVector<QVariant>> &values)
{
    const auto columnsSize = columns.size();

    QVector<QVariantMap> zippedValues;
    zippedValues.reserve(columnsSize);

    using SizeType = std::remove_cvref_t<decltype (columns)>::size_type;

    for (const auto &valuesList : values) {

        if (columnsSize != valuesList.size())
            throw Exceptions::InvalidArgumentError(
                QStringLiteral("A columns and values arguments don't have the same "
                               "number of items in %1().")
                .arg(__tiny_func__));

        QVariantMap zipped;

        for (SizeType i = 0; i < columnsSize; ++i)
            zipped.insert(columns[i], valuesList[i]);

        zippedValues << std::move(zipped);
    }

    return zippedValues;
}

int Query::queryResultSize(TSqlQuery &query)
{
    // Nothing to do, no result set, isn't active SELECT query
    if (!query.isActive() || !query.isSelect())
        return -1;

    if (query.driver()->hasFeature(TSqlDriver::QuerySize))
        return query.size();

    // Backup the current cursor position
    const auto currentCursor = query.at();

    query.seek(QSql::BeforeFirstRow);

    // Count manually
    int size = 0;
    while (query.next())
        ++size;

    // Restore the cursor position
    query.seek(currentCursor);

    return size;
}

} // namespace Orm::Utils

TINYORM_END_COMMON_NAMESPACE
