#include "orm/query/processors/postgresprocessor.hpp"

#include <QVariant>
#include <QtSql/QSqlQuery>

TINYORM_BEGIN_COMMON_NAMESPACE

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

TINYORM_END_COMMON_NAMESPACE
