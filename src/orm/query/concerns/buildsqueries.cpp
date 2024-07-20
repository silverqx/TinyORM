#include "orm/query/concerns/buildsqueries.hpp"

#include "orm/databaseconnection.hpp"
#include "orm/exceptions/multiplerecordsfounderror.hpp"
#include "orm/exceptions/recordsnotfounderror.hpp"
#include "orm/query/querybuilder.hpp"
#include "orm/utils/type.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

using QueryUtils = Orm::Utils::Query;

namespace Orm::Query::Concerns
{

/* public */

bool BuildsQueries::chunk(const qint64 count,
                          const std::function<bool(SqlQuery &, qint64)> &callback)
{
    builder().enforceOrderBy();

    qint64 page = 1;
    qint64 countResults = 0;

    do { // NOLINT(cppcoreguidelines-avoid-do-while)
        /* We'll execute the query for the given page and get the results. If there are
           no results we can just break and return from here. When there are results
           we will call the callback with the current chunk of these results here. */
        auto results = builder().forPage(page, count).get();

        countResults = QueryUtils::queryResultSize(results);

        if (countResults <= 0)
            break;

        /* On each chunk result set, we will pass them to the callback and then let the
           developer take care of everything within the callback, which allows us to
           keep the memory low for spinning through large result sets for working. */
        if (const auto result = std::invoke(callback, results, page);
            !result
        )
            return false;

        ++page;

    } while (countResults == count);

    return true;
}

bool BuildsQueries::each(const std::function<bool(SqlQuery &, qint64)> &callback,
                         const qint64 count)
{
    return chunk(count, [&callback](SqlQuery &results, const qint64 /*unused*/)
    {
        qint64 index = 0;

        while (results.next())
            if (const auto result = std::invoke(callback, results, index++);
                !result
            )
                return false;

        return true;
    });
}

/* This is trash as the QSqlQuery is passed to the callback, I need to pass something
   like std::map<std::pair<int, QString>, QVariant> so a user can modify it and return */
//QList<TSqlQuery>
//BuildsQueries::chunkMap(const std::function<void(TSqlQuery &)> &callback, const qint64 count)
//{
//    /* This method is weird, it should return one merged collection with all rows, but
//       it's impossible to merge more QSqlQuery-ies into the one QSqlQuery, so I have
//       decided to return the vector of these QSqlQueries.
//       It's not completely useless, only one difference will be that a user will have
//       to loop over all QSqlQuery-ies, instead of one big QSqlQuery.
//       Another confusing thing is that map-related algorithms are moving a value into
//       the callback (not non-const reference like here) and returning a new mapped value,
//       but it's not possible in this case as the QSqlQuery holds all other rows,
//       it's only a cursor. So I have to pass non-const reference and if all rows are
//       processed/looped then move a whole QSqlQuery into the result vector. */
//    QList<TSqlQuery> result;

//    chunk(count, [&result, &callback](TSqlQuery &results, const qint64 /*unused*/)
//    {
//        while (results.next())
//            std::invoke(callback, results);

//        result << std::move(results);

//        return true;
//    });

//    return result;
//}

bool BuildsQueries::chunkById(
        const qint64 count, const std::function<bool(SqlQuery &, qint64)> &callback,
        const QString &column, const QString &alias)
{
    const auto columnName = column.isEmpty() ? builder().defaultKeyName() : column;
    const auto aliasName = alias.isEmpty() ? columnName : alias;

    qint64 page = 1;
    qint64 countResults = 0;

    QVariant lastId;

    do { // NOLINT(cppcoreguidelines-avoid-do-while)
        auto clone = builder().clone();

        /* We'll execute the query for the given page and get the results. If there are
           no results we can just break and return from here. When there are results
           we will call the callback with the current chunk of these results here. */
        auto results = clone.forPageAfterId(count, lastId, columnName, true).get();

        countResults = QueryUtils::queryResultSize(results);

        if (countResults <= 0)
            break;

        /* Obtain the lastId before the results is passed to the user's callback because
           a user can leave the results (SqlQuery) in the invalid/changed state. */
        results.last();
        lastId = results.value(aliasName);
        // Restore a cursor position
        results.seek(TCursorPosition::BeforeFirstRow);

        /* And this check can also be made before a callback invocation, it saves
           the unnecessary invocation if the lastId is invalid. It also helps to avoid
           passing invalid data to the user. */
        if (!lastId.isValid() || lastId.isNull())
            throw Exceptions::RuntimeError(
                    QStringLiteral("The chunkById operation was aborted because the "
                                   "[%1] column is not present in the query result.")
                    .arg(aliasName));

        /* On each chunk result set, we will pass them to the callback and then let the
           developer take care of everything within the callback, which allows us to
           keep the memory low for spinning through large result sets for working. */
        if (const auto result = std::invoke(callback, results, page);
            !result
        )
            return false;

        ++page;

    } while (countResults == count);

    return true;
}

bool BuildsQueries::eachById(
        const std::function<bool(SqlQuery &, qint64)> &callback,
        const qint64 count, const QString &column, const QString &alias)
{
    return chunkById(count, [&callback, count](SqlQuery &results, const qint64 page)
    {
        qint64 index = 0;

        while (results.next())
            if (const auto result = std::invoke(callback, results,
                                                ((page - 1) * count) + index++);
                !result
            )
                return false;

        return true;
    }, column, alias);
}

SqlQuery BuildsQueries::sole(const QList<Column> &columns)
{
    auto query = builder().take(2).get(columns);

    if (builder().getConnection().pretending())
        return query;

    const auto count = QueryUtils::queryResultSize(query);

    if (count <= 0)
        throw Exceptions::RecordsNotFoundError(
                QStringLiteral("No records found in %1().").arg(__tiny_func__));

    if (count > 1)
        throw Exceptions::MultipleRecordsFoundError(count, __tiny_func__);

    query.first();

    return query;
}

Builder &BuildsQueries::tap(const std::function<void(Builder &)> &callback)
{
    std::invoke(callback, builder());

    return builder();
}

/* private */

Builder &BuildsQueries::builder() noexcept
{
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-static-cast-downcast)
    return static_cast<Builder &>(*this);
}

const Builder &BuildsQueries::builder() const noexcept
{
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-static-cast-downcast)
    return static_cast<const Builder &>(*this);
}

} // namespace Orm::Query::Concerns

TINYORM_END_COMMON_NAMESPACE
