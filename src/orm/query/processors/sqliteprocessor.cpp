#include "orm/query/processors/sqliteprocessor.hpp"

#include "orm/types/sqlquery.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Query::Processors
{

QStringList SQLiteProcessor::processColumnListing(SqlQuery &query) const
{
    QStringList columns;

    while (query.next())
        columns.append(query.value(NAME).value<QString>());

    return columns;
}

} // namespace Orm::Query::Processors

TINYORM_END_COMMON_NAMESPACE
