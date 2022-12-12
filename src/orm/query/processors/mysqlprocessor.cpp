#include "orm/query/processors/mysqlprocessor.hpp"

#include "orm/types/sqlquery.hpp"
#include "orm/utils/query.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

using QueryUtils = Orm::Utils::Query;

namespace Orm::Query::Processors
{

// DUP processor silverqx
QStringList MySqlProcessor::processColumnListing(SqlQuery &query) const
{
    QStringList columns;
    columns.reserve(QueryUtils::queryResultSize(query));

    while (query.next())
        columns.append(query.value("column_name").value<QString>());

    return columns;
}

} // namespace Orm::Query::Processors

TINYORM_END_COMMON_NAMESPACE
