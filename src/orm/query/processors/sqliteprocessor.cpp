#include "orm/query/processors/sqliteprocessor.hpp"

#include <QtSql/QSqlQuery>
#include <QVariant>

#include "orm/constants.hpp"

using namespace Orm::Constants;

TINYORM_BEGIN_COMMON_NAMESPACE

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
