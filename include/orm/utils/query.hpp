#pragma once
#ifndef ORM_UTILS_QUERY_HPP
#define ORM_UTILS_QUERY_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include <QVariant>

#include "orm/constants.hpp"
#include "orm/macros/commonnamespace.hpp"
#include "orm/macros/export.hpp"
#include "orm/utils/helpers.hpp"

class QSqlQuery;

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Utils
{

    /*! Concept for a bindings type used in the replaceBindingsInSql(). */
    template<typename T>
    concept BindingsConcept = std::convertible_to<T, QVector<QVariant>> ||
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
                              std::convertible_to<T, QVariantList>;
#else
                              std::convertible_to<T, QVariantMap>;
#endif

    /*! Library class for database query. */
    class SHAREDLIB_EXPORT Query
    {
        Q_DISABLE_COPY(Query)

        /*! Alias for the helper utils. */
        using Helpers = Orm::Utils::Helpers;

    public:
        /*! Deleted default constructor, this is a pure library class. */
        Query() = delete;
        /*! Deleted destructor. */
        ~Query() = delete;

        /*! Get the last executed query with replaced placeholders (ideal for logging). */
        static QString parseExecutedQuery(const QSqlQuery &query);
        /*! Get pretended query with replaced placeholders ( ideal for logging ). */
        inline static QString
        parseExecutedQueryForPretend(QString queryString,
                                     const QVector<QVariant> &bindings);

        /*! Replace all bindings in the given SQL query. */
        template<BindingsConcept T>
        static std::pair<QString, QStringList>
        replaceBindingsInSql(QString queryString, const T &bindings,
                             bool simpleBindings = false);

        /*! Log the last executed query to the debug output. */
        [[maybe_unused]]
        static void logExecutedQuery(const QSqlQuery &query);

        /*! Prepare the passed containers for the multi-insert. */
        static QVector<QVariantMap>
        zipForInsert(const QVector<QString> &columns,
                     const QVector<QVector<QVariant>> &values);

        /*! Returns the size of the result (number of rows returned). */
        static int queryResultSize(QSqlQuery &query);
    };

    /* public */

    QString Query::parseExecutedQueryForPretend(QString queryString,
                                                const QVector<QVariant> &bindings)
    {
        return replaceBindingsInSql(std::move(queryString), bindings).first;
    }

    template<BindingsConcept T>
    std::pair<QString, QStringList>
    Query::replaceBindingsInSql(QString queryString, const T &bindings,
                                const bool simpleBindings)
    {
        static const auto Invalid = QStringLiteral("INVALID");

        QString bindingValue;

        QStringList simpleBindingsList;
        simpleBindingsList.reserve(bindings.size());

        for (const auto &binding : bindings) {

            if (!binding.isValid())
                bindingValue = Invalid;
            else if (binding.isNull())
                bindingValue = Orm::Constants::null_;
            else
                // Support for string quoting
                bindingValue = (Helpers::qVariantTypeId(binding) == QMetaType::QString)
                               ? QStringLiteral("'%1'")
                                 .arg(binding.template value<QString>())
                               : binding.template value<QString>();

            queryString.replace(queryString.indexOf(QLatin1Char('?')), 1, bindingValue);

            if (simpleBindings)
                simpleBindingsList << bindingValue; // clazy:exclude=reserve-candidates
        }

        return {std::move(queryString), std::move(simpleBindingsList)};
    }

} // namespace Orm::Utils

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_UTILS_QUERY_HPP
