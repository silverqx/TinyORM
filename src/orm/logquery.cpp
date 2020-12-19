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

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    const auto boundValues = query.boundValues();
#else
    const auto boundValues = query.boundValues().values();
#endif

    for (int i = 0; i < boundValues.size(); ++i) {
        const auto boundValueRaw = boundValues.at(i);
        QString boundValue;

        if (boundValueRaw.isNull())
            boundValue = "null";
        else if (!boundValueRaw.isValid())
            boundValue = "INVALID";
        else
            // Support for string quoting
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
            boundValue = (boundValueRaw.typeId() == QMetaType::QString)
#else
            boundValue = (boundValueRaw.userType() == QMetaType::QString)
#endif
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
