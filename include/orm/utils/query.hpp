#pragma once
#ifndef ORM_UTILS_QUERY_HPP
#define ORM_UTILS_QUERY_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include <QVariant>

#include "orm/constants.hpp"
#include "orm/macros/commonnamespace.hpp"
#include "orm/macros/export.hpp"
#include "orm/macros/sqldrivermappings.hpp"
#include "orm/support/replacebindings.hpp"

TINY_FORWARD_DECLARE_TSqlQuery

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Utils
{

    /*! Library class for database query. */
    class TINYORM_EXPORT Query : public Support::ReplaceBindings
    {
        Q_DISABLE_COPY_MOVE(Query)

    public:
        /*! Deleted default constructor, this is a pure library class. */
        Query() = delete;
        /*! Deleted destructor. */
        ~Query() = delete;

        /*! Get the last executed query with replaced placeholders (ideal for logging). */
        static QString parseExecutedQuery(const TSqlQuery &query);
        /*! Get pretended query with replaced placeholders ( ideal for logging ). */
        inline static QString
        parseExecutedQueryForPretend(QString queryString,
                                     const QList<QVariant> &bindings);

        /*! Log the last executed query to the debug output. */
        [[maybe_unused]]
        static void logExecutedQuery(const TSqlQuery &query);

        /*! Prepare the passed containers for the multi-insert. */
        static QList<QVariantMap>
        zipForInsert(const QList<QString> &columns,
                     const QList<QList<QVariant>> &values);

        /*! Returns the size of the result (number of rows returned). */
        static int queryResultSize(TSqlQuery &query);
    };

    /* public */

    QString Query::parseExecutedQueryForPretend(QString queryString,
                                                const QList<QVariant> &bindings)
    {
        return replaceBindingsInSql(std::move(queryString), bindings).first;
    }

} // namespace Orm::Utils

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_UTILS_QUERY_HPP
