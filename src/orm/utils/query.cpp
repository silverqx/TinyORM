#include "orm/utils/query.hpp"

#include <QDebug>
#include <QtSql/QSqlQuery>

#include "orm/exceptions/invalidargumenterror.hpp"
#include "orm/utils/type.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Utils
{

namespace
{
    const auto null_SL    = QStringLiteral("null"); // clazy:exclude=non-pod-global-static
    const auto INVALID_SL = QStringLiteral("INVALID"); // clazy:exclude=non-pod-global-static
}

QString Query::parseExecutedQuery(const QSqlQuery &query)
{
    auto executedQuery = query.executedQuery();
    if (executedQuery.isEmpty())
        executedQuery = query.lastQuery();

    QString boundValue;

    for (auto &&boundValueRaw : query.boundValues()) {

        if (boundValueRaw.isNull())
            boundValue = null_SL;
        else if (!boundValueRaw.isValid())
            boundValue = INVALID_SL;
        else
            // Support for string quoting
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
            boundValue = (boundValueRaw.typeId() == QMetaType::QString)
#else
            boundValue = (boundValueRaw.userType() == QMetaType::QString)
#endif
                         ? QStringLiteral("\"%1\"").arg(boundValueRaw.value<QString>())
                         : boundValueRaw.value<QString>();

        executedQuery.replace(executedQuery.indexOf(QChar('?')), 1, boundValue);
    }

    return executedQuery;
}

QString Query::parseExecutedQueryForPretend(QString query,
                                            const QVector<QVariant> &bindings)
{
    QString boundValue;

    for (const auto &bindingRaw : bindings) {

        if (bindingRaw.isNull())
            boundValue = null_SL;
        else if (!bindingRaw.isValid())
            boundValue = INVALID_SL;
        else
            // Support for string quoting
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
            boundValue = (bindingRaw.typeId() == QMetaType::QString)
#else
            boundValue = (bindingRaw.userType() == QMetaType::QString)
#endif
                         ? QStringLiteral("\"%1\"").arg(bindingRaw.value<QString>())
                         : bindingRaw.value<QString>();

        query.replace(query.indexOf(QChar('?')), 1, boundValue);
    }

    return query;
}

#if !defined(TINYORM_NO_DEBUG)
void Query::logExecutedQuery(const QSqlQuery &query)
{
    qDebug().noquote() << QStringLiteral("Executed Query :")
                       << Query::parseExecutedQuery(query);
}
#else
void Query::logExecutedQuery(const QSqlQuery &/*unused*/)
{}
#endif

QVector<QVariantMap>
Query::zipForInsert(const QVector<QString> &columns, QVector<QVector<QVariant>> values)
{
    const auto columnsSize = columns.size();

    QVector<QVariantMap> zippedValues;
    zippedValues.reserve(columnsSize);

    using SizeType = std::remove_cvref_t<decltype (columns)>::size_type;

    for (auto &&valuesList : values) {

        if (columnsSize != valuesList.size())
            throw Exceptions::InvalidArgumentError(
                QStringLiteral("A columns and values arguments don't have the same "
                               "number of items in %1.")
                .arg(__tiny_func__));

        QVariantMap zipped;

        for (SizeType i = 0; i < columnsSize; ++i)
            zipped.insert(columns[i], std::move(valuesList[i]));

        zippedValues << std::move(zipped);
    }

    return zippedValues;
}

} // namespace Orm::Utils

TINYORM_END_COMMON_NAMESPACE
