#include "orm/query/processors/postgresprocessor.hpp"

#include <QtSql/QSqlQuery>
#include <QVariant>

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm::Query::Processors
{

QStringList PostgresProcessor::processColumnListing(QSqlQuery &query) const
{
    QStringList columns;

    while (query.next())
        columns.append(query.value("column_name").value<QString>());

    return columns;
}

} // namespace Orm::Query::Processors
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif
