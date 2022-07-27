#pragma once
#ifndef ORM_QUERY_CONCERNS_BUILDSQUERIES_HPP
#define ORM_QUERY_CONCERNS_BUILDSQUERIES_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include "orm/databaseconnection.hpp"
#include "orm/exceptions/multiplerecordsfounderror.hpp"
#include "orm/exceptions/recordsnotfounderror.hpp"
#include "orm/utils/query.hpp"
#include "orm/utils/type.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Query::Concerns
{

    /*! Concept for the results used in the Concern::BuildsQueries. */
    template<typename T>
    concept BuildsQueriesResults = std::same_as<T, QSqlQuery>;

    // TODO buildsqueries, missing chunkMap() silverqx
    /*! More complex 'Retrieving results' methods that internally build queries. */
    template<BuildsQueriesResults T>
    class BuildsQueries // clazy:exclude=copyable-polymorphic
    {
        /*! Alias for the query utils. */
        using QueryUtils = Orm::Utils::Query;

    public:
        /*! Default constructor. */
        inline BuildsQueries() = default;
        /*! Virtual destructor, to pass -Weffc++. */
        inline virtual ~BuildsQueries() = default;

        /*! Copy constructor. */
        inline BuildsQueries(const BuildsQueries &) = default;
        /*! Deleted copy assignment operator (QueryBuilder class constains reference and
            const). */
        BuildsQueries &operator=(const BuildsQueries &) = delete;

        /*! Move constructor. */
        inline BuildsQueries(BuildsQueries &&) noexcept = default;
        /*! Deleted move assignment operator (QueryBuilder class constains reference and
            const). */
        BuildsQueries &operator=(BuildsQueries &&) = delete;

        /*! Chunk the results of the query. */
        bool chunk(int count,
                   const std::function<bool(T &results, int page)> &callback);
        /*! Execute a callback over each item while chunking. */
        bool each(const std::function<bool(T &row, int index)> &callback,
                  int count = 1000);

        /*! Run a map over each item while chunking. */
//        QVector<T>
//        chunkMap(const std::function<void(T &row)> &callback, int count = 1000);

        /*! Chunk the results of a query by comparing IDs. */
        bool chunkById(int count,
                       const std::function<bool(T &results, int page)> &callback,
                       const QString &column = "", const QString &alias = "");
        /*! Execute a callback over each item while chunking by ID. */
        bool eachById(const std::function<bool(T &row, int index)> &callback,
                  int count = 1000, const QString &column = "",
                  const QString &alias = "");

        /*! Execute the query and get the first result if it's the sole matching
            record. */
        T sole(const QVector<Column> &columns = {ASTERISK});

        /*! Pass the query to a given callback. */
        Builder &tap(const std::function<void(Builder &query)> &callback);

    private:
        /*! Static cast *this to the QueryBuilder & derived type. */
        Builder &builder();
        /*! Static cast *this to the QueryBuilder & derived type, const version. */
        const Builder &builder() const;
    };

    /* public */

    template<BuildsQueriesResults T>
    bool BuildsQueries<T>::chunk(const int count,
                                 const std::function<bool(T &, int)> &callback)
    {
        builder().enforceOrderBy();

        int page = 1;
        int countResults = 0;

        do {
            /* We'll execute the query for the given page and get the results. If there
               are no results we can just break and return. When there are results
               we will call the callback with the current chunk of these results. */
            auto results = builder().forPage(page, count).get();

            countResults = QueryUtils::queryResultSize(results);

            if (countResults == 0)
                break;

            /* On each chunk result set, we will pass them to the callback and then let
               the developer take care of everything within the callback, which allows
               us to keep the memory low for spinning through large result sets
               for working. */
            if (const auto result = std::invoke(callback, results, page);
                !result
            )
                return false;

            ++page;

        } while (countResults == count);

        return true;
    }

    template<BuildsQueriesResults T>
    bool BuildsQueries<T>::each(const std::function<bool(T &, int)> &callback,
                                const int count)
    {
        return chunk(count, [&callback](T &results, const int /*unused*/)
        {
            int index = 0;

            while (results.next())
                if (const auto result = std::invoke(callback, results, index++);
                    !result
                )
                    return false;

            return true;
        });
    }

    /* This is trash as the QSqlQuery is passed to the callback, I need to pass something
       like std::map<std::pair<int, QString>, QVariant> so an user can modify it and
       return. */
//    template<BuildsQueriesResults T>
//    QVector<QSqlQuery>
//    BuildsQueries<T>::chunkMap(const std::function<void(QSqlQuery &)> &callback,
//                               const int count)
//    {
//        /* This method is weird, it should return one merged collection with all rows,
//           but it's impossible to merge more QSqlQuery-ies into the one QSqlQuery, so
//           I have decided to return the vector of these QSqlQueries.
//           It's not completely useless, only one difference will be that an user will
//           have to loop over all QSqlQuery-ies, instead of one big QSqlQuery.
//           Another confusing thing is that map-related algorithms are moving a value
//           into the callback (not non-const reference like here) and returning
//           a new mapped value, but it's not possible in this case as the QSqlQuery holds
//           all other rows, it's only a cursor. So I have to pass non-const reference and
//           if all rows are processed/looped then move a whole QSqlQuery into the result
//           vector. */
//        QVector<QSqlQuery> result;

//        chunk(count, [&result, &callback](QSqlQuery &results, const int /*unused*/)
//        {
//            while (results.next())
//                std::invoke(callback, results);

//            result << std::move(results);

//            return true;
//        });

//        return result;
//    }

    template<BuildsQueriesResults T>
    bool BuildsQueries<T>::chunkById(
            const int count, const std::function<bool(T &, int)> &callback,
            const QString &column, const QString &alias)
    {
        const auto columnName = column.isEmpty() ? builder().defaultKeyName() : column;
        const auto aliasName = alias.isEmpty() ? columnName : alias;

        int page = 1;
        int countResults = 0;

        QVariant lastId;

        do {
            auto clone = builder().clone();

            /* We'll execute the query for the given page and get the results. If there are
               no results we can just break and return. When there are results we will
               call the callback with the current chunk of these results. */
            auto results = clone.forPageAfterId(count, lastId, columnName, true).get();

            countResults = QueryUtils::queryResultSize(results);

            if (countResults == 0)
                break;

            /* Obtain the lastId before the results is passed to the user's callback
               because an user can leave the results (QSqlQuery) in the invalid state. */
            results.last();
            lastId = results.value(aliasName);
            // Restore a cursor position
            results.seek(QSql::BeforeFirstRow);

            /* And the check can also be made before a callback invocation, it saves
               the unnecessary invocation if the lastId is invalid. It also helps
               to avoid passing invalid data to the user. */
            if (!lastId.isValid() || lastId.isNull())
                throw Exceptions::RuntimeError(
                        QStringLiteral(
                            "The chunkById operation was aborted because the "
                            "[%1] column is not present in the query result.")
                        .arg(aliasName));

            /* On each chunk result set, we will pass them to the callback and then let
               the developer take care of everything within the callback, which allows
               us to keep the memory low for spinning through large result sets
               for working. */
            if (const auto result = std::invoke(callback, results, page);
                !result
            )
                return false;

            ++page;

        } while (countResults == count);

        return true;
    }

    template<BuildsQueriesResults T>
    bool BuildsQueries<T>::eachById(
            const std::function<bool(T &, int)> &callback,
            const int count, const QString &column, const QString &alias)
    {
        return chunkById(count, [&callback, count](T &results, const int page)
        {
            int index = 0;

            while (results.next())
                if (const auto result = std::invoke(callback, results,
                                                    ((page - 1) * count) + index++);
                    !result
                )
                    return false;

            return true;
        },
                column, alias);
    }

    // CUR buildsqueries, check if all are pretending compatible silverqx
    template<BuildsQueriesResults T>
    T BuildsQueries<T>::sole(const QVector<Column> &columns)
    {
        auto query = builder().take(2).get(columns);

        if (builder().getConnection().pretending())
            return query;

        const auto count = QueryUtils::queryResultSize(query);

        if (count == 0)
            throw Exceptions::RecordsNotFoundError(
                    QStringLiteral("No records found in %1().").arg(__tiny_func__));

        if (count > 1)
            throw Exceptions::MultipleRecordsFoundError(count);

        query.first();

        return query;
    }

    template<BuildsQueriesResults T>
    Builder &BuildsQueries<T>::tap(const std::function<void(Builder &)> &callback)
    {
        std::invoke(callback, builder());

        return builder();
    }

    /* private */

    template<BuildsQueriesResults T>
    Builder &BuildsQueries<T>::builder()
    {
        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-static-cast-downcast)
        return static_cast<Builder &>(*this);
    }

    template<BuildsQueriesResults T>
    const Builder &BuildsQueries<T>::builder() const
    {
        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-static-cast-downcast)
        return static_cast<const Builder &>(*this);
    }

} // namespace Orm::Query::Concerns

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_QUERY_CONCERNS_BUILDSQUERIES_HPP
