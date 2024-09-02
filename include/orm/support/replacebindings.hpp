#pragma once
#ifndef ORM_SUPPORT_REPLACEBINDINGS_HPP
#define ORM_SUPPORT_REPLACEBINDINGS_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include <QVariant>

#include "orm/macros/commonnamespace.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Support
{

    /*! Concept for a bindings type used in the replaceBindingsInSql(). */
    template<typename T>
    concept BindingsConcept = std::convertible_to<T, QList<QVariant>> ||
                              std::convertible_to<T, QVariantList>;

    /*! Library class for replace placeholders in SQL queries. */
    class ReplaceBindings
    {
        Q_DISABLE_COPY_MOVE(ReplaceBindings)

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
        using namespace Qt::StringLiterals;

        // Can't use orm/constants.hpp because this class will be also used in TinyDrivers
        static const auto Invalid     = u"INVALID"_s;
        static const auto Null_       = u"null"_s;
        static const auto Binary      = u"<binary(%1)>"_s;
        static const auto HexBinary   = u"hex-binary(%1)'%2'"_s;
        static const auto TextAlt     = u"<text(%1)>"_s;
        static const auto TmplSQuotes = u"'%1'"_s;

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
            else if (binding.typeId() == QMetaType::QByteArray) {
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
            else if (binding.typeId() == QMetaType::QString) {
                const auto textData = binding.template value<QString>();
                const auto textSize = textData.size();
                // Don't overwhelm terminal with a lot of text, 1024 characters is enough
                bindingValue = textSize > 1024 ? TextAlt.arg(textSize)
                                               : TmplSQuotes.arg(textData);
            }
            else
                bindingValue = binding.template value<QString>();

            using Qt::StringLiterals::operator""_L1;

            queryString.replace(queryString.indexOf('?'_L1), 1, bindingValue);

            if (simpleBindings)
                simpleBindingsList << std::move(bindingValue); // clazy:exclude=reserve-candidates
        }

        return {std::move(queryString), std::move(simpleBindingsList)};
    }

} // namespace Orm::Support

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_SUPPORT_REPLACEBINDINGS_HPP
