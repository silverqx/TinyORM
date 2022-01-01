#pragma once
#ifndef ORM_UTILS_QUERY_HPP
#define ORM_UTILS_QUERY_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include <QString>
#include <QVariant>

#include "orm/macros/commonnamespace.hpp"
#include "orm/macros/export.hpp"

class QSqlQuery;

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Utils
{

    /*! Library class for database query. */
    class SHAREDLIB_EXPORT Query
    {
        Q_DISABLE_COPY(Query)

    public:
        /*! Deleted default constructor, this is a pure library class. */
        Query() = delete;
        /*! Deleted destructor. */
        ~Query() = delete;

        /*! Get the last executed query with replaced placeholders (ideal for logging). */
        static QString parseExecutedQuery(const QSqlQuery &query);
        /*! Get pretended query with replaced placeholders ( ideal for logging ). */
        static QString
        parseExecutedQueryForPretend(QString query, const QVector<QVariant> &bindings);

        /*! Log the last executed query to the debug output. */
        [[maybe_unused]]
        static void logExecutedQuery(const QSqlQuery &query);
    };

} // namespace Orm::Utils

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_UTILS_QUERY_HPP
