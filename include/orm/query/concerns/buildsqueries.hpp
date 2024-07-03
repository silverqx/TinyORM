#pragma once
#ifndef ORM_QUERY_CONCERNS_BUILDSQUERIES_HPP
#define ORM_QUERY_CONCERNS_BUILDSQUERIES_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include "orm/types/sqlquery.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Query::Concerns
{

    /*! More complex 'Retrieving results' methods that internally build queries. */
    class TINYORM_EXPORT BuildsQueries // clazy:exclude=copyable-polymorphic
    {
    public:
        /*! Default constructor. */
        BuildsQueries() = default;
        /*! Pure virtual destructor, to pass -Weffc++. */
        inline virtual ~BuildsQueries() = 0;

        /*! Copy constructor. */
        BuildsQueries(const BuildsQueries &) = default;
        /*! Deleted copy assignment operator (QueryBuilder class contains reference and
            const). */
        BuildsQueries &operator=(const BuildsQueries &) = delete;

        /*! Move constructor. */
        BuildsQueries(BuildsQueries &&) = default;
        /*! Deleted move assignment operator (QueryBuilder class contains reference and
            const). */
        BuildsQueries &operator=(BuildsQueries &&) = delete;

        /*! Chunk the results of the query. */
        bool chunk(qint64 count,
                   const std::function<bool(SqlQuery &results, qint64 page)> &callback);
        /*! Execute a callback over each item while chunking. */
        bool each(const std::function<bool(SqlQuery &row, qint64 index)> &callback,
                  qint64 count = 1000);

        /*! Run a map over each item while chunking. */
//        QList<TSqlQuery>
//        chunkMap(const std::function<void(TSqlQuery &row)> &callback, qint64 count = 1000);

        /*! Chunk the results of a query by comparing IDs. */
        bool chunkById(qint64 count,
                       const std::function<
                           bool(SqlQuery &results, qint64 page)> &callback,
                       const QString &column = "", const QString &alias = "");
        /*! Execute a callback over each item while chunking by ID. */
        bool eachById(const std::function<bool(SqlQuery &row, qint64 index)> &callback,
                      qint64 count = 1000, const QString &column = "",
                      const QString &alias = "");


        /*! Execute the query and get the first result if it's the sole matching
            record. */
        SqlQuery sole(const QList<Column> &columns = {ASTERISK});

        /*! Pass the query to a given callback. */
        Builder &tap(const std::function<void(Builder &query)> &callback);

    private:
        /*! Static cast *this to the QueryBuilder & derived type. */
        Builder &builder() noexcept;
        /*! Static cast *this to the QueryBuilder & derived type, const version. */
        const Builder &builder() const noexcept;
    };

    /* public */

    BuildsQueries::~BuildsQueries() = default;

} // namespace Orm::Query::Concerns

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_QUERY_CONCERNS_BUILDSQUERIES_HPP
