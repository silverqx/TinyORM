#pragma once
#ifndef ORM_QUERY_CONCERNS_BUILDSQUERIES_HPP
#define ORM_QUERY_CONCERNS_BUILDSQUERIES_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include "orm/macros/export.hpp"
#include "orm/ormtypes.hpp"

class QSqlQuery;

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Query
{
    class Builder;

namespace Concerns
{

    // TODO buildsqueries, missing chunkMap() silverqx
    /*! More complex 'Retrieving results' methods that internally build queries. */
    class SHAREDLIB_EXPORT BuildsQueries // clazy:exclude=copyable-polymorphic
    {
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
        inline BuildsQueries(BuildsQueries &&) = default;
        /*! Deleted move assignment operator (QueryBuilder class constains reference and
            const). */
        BuildsQueries &operator=(BuildsQueries &&) = delete;

        /*! Chunk the results of the query. */
        bool chunk(int count,
                   const std::function<bool(QSqlQuery &results, int page)> &callback);
        /*! Execute a callback over each item while chunking. */
        bool each(const std::function<bool(QSqlQuery &row, int index)> &callback,
                  int count = 1000);

        /*! Run a map over each item while chunking. */
//        QVector<QSqlQuery>
//        chunkMap(const std::function<void(QSqlQuery &row)> &callback, int count = 1000);

        /*! Chunk the results of a query by comparing IDs. */
        bool chunkById(int count,
                       const std::function<bool(QSqlQuery &results, int page)> &callback,
                       const QString &column = "", const QString &alias = "");
        /*! Execute a callback over each item while chunking by ID. */
        bool eachById(const std::function<bool(QSqlQuery &row, int index)> &callback,
                  int count = 1000, const QString &column = "",
                  const QString &alias = "");


        /*! Execute the query and get the first result if it's the sole matching
            record. */
        QSqlQuery sole(const QVector<Column> &columns = {ASTERISK});

        /*! Pass the query to a given callback. */
        Builder &tap(const std::function<void(Builder &query)> &callback);

    private:
        /*! Static cast *this to the QueryBuilder & derived type. */
        Builder &builder();
        /*! Static cast *this to the QueryBuilder & derived type, const version. */
        const Builder &builder() const;
    };

} // namespace Concerns
} // namespace Orm::Query

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_QUERY_CONCERNS_BUILDSQUERIES_HPP
