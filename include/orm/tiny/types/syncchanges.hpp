#pragma once
#ifndef ORM_TINY_TYPES_SYNCCHANGES_HPP
#define ORM_TINY_TYPES_SYNCCHANGES_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include <QVariant>
#include <QVector>

#include <map>

#if defined(__clang__) || (defined(_MSC_VER) && _MSC_VER <= 1928)
#  include <range/v3/algorithm/unique.hpp>
#endif

#include "orm/macros/commonnamespace.hpp"
#include "orm/macros/export.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Tiny
{
namespace Types
{

    /*! Result of the sync() related methods in belongs to many relation type. */
    class SHAREDLIB_EXPORT SyncChanges final :
            public std::map<QString, QVector<QVariant>>
    {
    public:
        /*! Constructor. */
        SyncChanges();

        /*! Merge changes into the current instance. */
        template<typename KeyType>
        SyncChanges &merge(SyncChanges &&changes);
        /*! Determine if the given key is supported. */
        bool supportedKey(const QString &key) const;

    protected:
        /*! Cast the given key to primary key type. */
        template<typename T>
        inline T castKey(const QVariant &key) const;

    private:
        /*! All of the supported keys. */
        inline static const QVector<QString> SyncKeys {
            QStringLiteral("attached"),
            QStringLiteral("detached"),
            QStringLiteral("updated")
        };
    };

    template<typename KeyType>
    SyncChanges &SyncChanges::merge(SyncChanges &&changes)
    {
        for (auto &&[key, values] : changes) {
            {
                auto &currentValues = (*this)[key];

                // If the current key value is empty, then simply move a new values
                if (supportedKey(key) && currentValues.isEmpty()) {
                    if (!values.isEmpty())
                        (*this)[key] = std::move(values);

                    continue;
                }
            }

            // Otherwise merge values
            const auto castKey = [this](const auto &id)
            {
                return this->castKey<KeyType>(id);
            };

            /* First we need to make a copy and then sort both values, vectors
               have to be sorted before the merge. */
            auto currentValues = (*this)[key];
            std::ranges::sort(currentValues, {}, castKey);
            std::ranges::sort(values, {}, castKey);

            // Then merge two vectors
            QVector<QVariant> merged;
            merged.reserve(currentValues.size() + values.size());
            std::ranges::merge(currentValues, values, std::back_inserter(merged),
                               {}, castKey, castKey);

            // Remove duplicates
            // BUG clang doesn't compile with std::ranges::unique, I don't unserstand why silverqx
#if (defined(__GNUG__) && !defined(__clang__)) || (defined(_MSC_VER) && _MSC_VER >= 1929)
            auto [first, last] = std::ranges::unique(merged, {}, castKey);
            merged.erase(first, last);
#else
            auto it = ranges::unique(merged, {}, castKey);
            merged.erase(it, ranges::end(merged));
#endif

            (*this)[key].swap(merged);
        }

        return *this;
    }

    template<typename T>
    T SyncChanges::castKey(const QVariant &key) const
    {
        return key.value<T>();
    }

} // namespace Types

    using SyncChanges = Tiny::Types::SyncChanges;

} // namespace Orm::Tiny

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_TINY_TYPES_SYNCCHANGES_HPP
