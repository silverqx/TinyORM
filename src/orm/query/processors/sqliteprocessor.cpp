#include "orm/query/processors/sqliteprocessor.hpp"

#include "orm/types/sqlquery.hpp"
#include "orm/utils/query.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

using QueryUtils = Orm::Utils::Query;

namespace Orm::Query::Processors
{

QStringList SQLiteProcessor::processColumnListing(SqlQuery &query) const
{
    QStringList columns;
    columns.reserve(QueryUtils::queryResultSize(query));

    while (query.next())
        columns << query.value(NAME).value<QString>();

    return columns;
}

} // namespace Orm::Query::Processors

TINYORM_END_COMMON_NAMESPACE
