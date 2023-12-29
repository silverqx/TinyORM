#include "orm/concerns/explainqueries.hpp"

#include "orm/databaseconnection.hpp"
#include "orm/query/querybuilder.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Concerns
{

/* public */

// BUG Qt sql driver does not support to call EXPLAIN as a prepared statement, look at enum StatementType and QSqlDriver::sqlStatement in qsqldriver.h/cpp, also don't forget to add proxies when Qt will support EXPLAIN queries silverqx
TSqlQuery ExplainQueries::explain()
{
    return builder().getConnection().select(
                QStringLiteral("EXPLAIN %1").arg(builder().toSql()),
                builder().getBindings());
}

/* private */

QueryBuilder &ExplainQueries::builder()
{
    return dynamic_cast<QueryBuilder &>(*this);
}

} // namespace Orm::Concerns

TINYORM_END_COMMON_NAMESPACE
