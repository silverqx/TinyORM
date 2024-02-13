#pragma once
#ifndef ORM_SUPPORT_REPLACEBINDINGS_HPP
#define ORM_SUPPORT_REPLACEBINDINGS_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include <QVariant>

#include "orm/macros/commonnamespace.hpp"

#ifdef PROJECT_TINYDRIVERS_PRIVATE
#  include "orm/drivers/utils/helpers_p.hpp"
#else
#  include "orm/utils/helpers.hpp"
#endif

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Support
{

    /*! Concept for a bindings type used in the replaceBindingsInSql(). */
    template<typename T>
    concept BindingsConcept = std::convertible_to<T, QVector<QVariant>> ||
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
                              std::convertible_to<T, QVariantList>;
#else
                              std::convertible_to<T, QVariantMap>;
#endif

    /*! Library class for replace placeholders in SQL queries. */
    class ReplaceBindings
    {
        Q_DISABLE_COPY_MOVE(ReplaceBindings)

#ifdef PROJECT_TINYDRIVERS_PRIVATE
        /*! Alias for the helper utils. */
        using Helpers = Orm::Drivers::Utils::Helpers;
#else
        /*! Alias for the helper utils. */
        using Helpers = Orm::Utils::Helpers;
#endif

    public:
        /*! Deleted default constructor, this is a pure library class. */
        ReplaceBindings() = delete;
        /*! Deleted destructor. */
        ~ReplaceBindings() = delete;

        /*! Replace all bindings in the given SQL query. */
        template<BindingsConcept T>
        static std::pair<QString, QStringList>
        replaceBindingsInSql(QString queryString, const T &bindings,
                             bool simpleBindings = false);
    };

    /* public */

    template<BindingsConcept T>
    std::pair<QString, QStringList>
    ReplaceBindings::replaceBindingsInSql(QString queryString, const T &bindings,
                                          const bool simpleBindings)
    {
        // Can't use orm/constants.hpp because this class will be also used in TinyDrivers
        static const auto Invalid      = QStringLiteral("INVALID");
        static const auto Null_        = QStringLiteral("null");
        static const auto Binary       = QStringLiteral("<binary(%1)>");
        static const auto HexBinary    = QStringLiteral("hex-binary(%1)\"%2\"");
        static const auto TMPL_SQUOTES = QStringLiteral("\"%1\"");

        QString bindingValue;

        QStringList simpleBindingsList;
        if (simpleBindings)
            simpleBindingsList.reserve(bindings.size());

        for (const auto &binding : bindings) {

            if (!binding.isValid())
                bindingValue = Invalid;

            else if (binding.isNull())
                bindingValue = Null_;

            // Support for binary data (BLOB)
            else if (Helpers::qVariantTypeId(binding) == QMetaType::QByteArray) {
                const auto binaryData = binding.template value<QByteArray>();
                const auto binarySize = binaryData.size();
                // Don't overwhelm terminal with a lot of text, 512 characters is enough
                bindingValue = binarySize > 512
                               ? Binary.arg(binarySize)
                               : HexBinary.arg(binarySize)
                                          .arg(QString::fromLatin1(
                                                   binaryData.toHex(' ')));
            }
            // Support for strings quoting
            else if (Helpers::qVariantTypeId(binding) == QMetaType::QString)
                bindingValue = TMPL_SQUOTES.arg(binding.template value<QString>());
            else
                bindingValue = binding.template value<QString>();

            queryString.replace(queryString.indexOf(QLatin1Char('?')), 1, bindingValue);

            if (simpleBindings)
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
                simpleBindingsList << std::move(bindingValue);
#else
                simpleBindingsList << bindingValue;
#endif
        }

        return {std::move(queryString), std::move(simpleBindingsList)};
    }

} // namespace Orm::Support

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_SUPPORT_REPLACEBINDINGS_HPP
