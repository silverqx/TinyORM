#include "orm/utils/query.hpp"

#include <QDebug>
#include <QtSql/QSqlDriver>
#include <QtSql/QSqlQuery>

#include "orm/exceptions/invalidargumenterror.hpp"
#include "orm/utils/type.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Utils
{

/* We don't need the Orm::SqlQuery overloads for the parseExecutedQuery() and
   logExecutedQuery() as all bindings are already prepared. */

QString Query::parseExecutedQuery(const QSqlQuery &query)
{
    auto executedQuery = query.executedQuery();
    if (executedQuery.isEmpty())
        executedQuery = query.lastQuery();

    return replaceBindingsInSql(std::move(executedQuery), query.boundValues()).first;
}

#if !defined(TINYORM_NO_DEBUG)
void Query::logExecutedQuery(const QSqlQuery &query)
{
    qDebug().noquote() << QStringLiteral("Executed Query :")
                       << Query::parseExecutedQuery(query);
}
#else
void Query::logExecutedQuery(const QSqlQuery &/*unused*/)
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

int Query::queryResultSize(QSqlQuery &query)
{
    if (query.driver()->hasFeature(QSqlDriver::QuerySize))
        return query.size();

    query.seek(QSql::BeforeFirstRow);

    // Count manually
    int size = 0;
    while (query.next())
        ++size;

    query.seek(QSql::BeforeFirstRow);

    return size;
}

} // namespace Orm::Utils

TINYORM_END_COMMON_NAMESPACE
