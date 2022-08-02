#include "orm/query/processors/sqliteprocessor.hpp"

#include <QVariant>
#include <QtSql/QSqlQuery>

#include "orm/constants.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

using Orm::Constants::NAME;

namespace Orm::Query::Processors
{

QStringList SQLiteProcessor::processColumnListing(QSqlQuery &query) const
{
    QStringList columns;

    while (query.next())
        columns.append(query.value(NAME).value<QString>());

    return columns;
}

} // namespace Orm::Query::Processors

TINYORM_END_COMMON_NAMESPACE
