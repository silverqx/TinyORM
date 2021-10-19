#include "orm/logquery.hpp"

#include <QDebug>
#include <QtSql/QSqlQuery>

// CUR not a good place for this file, move it to the utils or support silverqx
TINYORM_BEGIN_COMMON_NAMESPACE

QString parseExecutedQuery(const QSqlQuery &query)
{
    auto executedQuery = query.executedQuery();
    if (executedQuery.isEmpty())
        executedQuery = query.lastQuery();

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    const auto boundValues = query.boundValues();
#else
    const auto boundValues = query.boundValues().values(); // clazy:exclude=inefficient-qlist
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
                         ? QStringLiteral("\"%1\"").arg(boundValueRaw.value<QString>())
                         : boundValueRaw.value<QString>();

        executedQuery.replace(executedQuery.indexOf('?'), 1, boundValue);
    }

    return executedQuery;
}

QString parseExecutedQueryForPretend(QString query, const QVector<QVariant> &bindings)
{
    QString boundValue;

    for (const auto &bindingRaw : bindings) {

        if (bindingRaw.isNull())
            boundValue = "null";
        else if (!bindingRaw.isValid())
            boundValue = "INVALID";
        else
            // Support for string quoting
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
            boundValue = (bindingRaw.typeId() == QMetaType::QString)
#else
            boundValue = (bindingRaw.userType() == QMetaType::QString)
#endif
                         ? QStringLiteral("\"%1\"").arg(bindingRaw.value<QString>())
                         : bindingRaw.value<QString>();

        query.replace(query.indexOf('?'), 1, boundValue);
    }

    return query;
}

void logExecutedQuery(const QSqlQuery &query)
{
    qDebug().noquote() << QStringLiteral("Executed Query :")
                       << parseExecutedQuery(query);
}

TINYORM_END_COMMON_NAMESPACE
