#include "logquery.h"

#include <QDebug>
#include <QtSql/QSqlQuery>

#ifdef MANGO_COMMON_NAMESPACE
namespace MANGO_COMMON_NAMESPACE
{
#endif

QString parseExecutedQuery(const QSqlQuery &query)
{
    auto executedQuery = query.executedQuery();
    const auto boundValues = query.boundValues().values();

    for (int i = 0; i < boundValues.size(); ++i) {
        const auto boundValueRaw = boundValues.at(i);
        QString boundValue;

        if (boundValueRaw.isNull())
            boundValue = "null";
        else if (!boundValueRaw.isValid())
            boundValue = "INVALID";
        else
            // Support for string quoting
            boundValue = (boundValueRaw.type() == QVariant::Type::String)
                         ? QStringLiteral("\"%1\"").arg(boundValueRaw.toString())
                         : boundValueRaw.toString();

        executedQuery.replace(executedQuery.indexOf('?'), 1, boundValue);
    }

    return executedQuery;
}

void logExecutedQuery(const QSqlQuery &query)
{
    qDebug().noquote() << QStringLiteral("Executed Query :")
                       << parseExecutedQuery(query);
}

#ifdef MANGO_COMMON_NAMESPACE
}
#endif
