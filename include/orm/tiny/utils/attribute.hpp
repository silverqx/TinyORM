#pragma once
#ifndef ORM_TINY_UTILS_ATTRIBUTE_HPP
#define ORM_TINY_UTILS_ATTRIBUTE_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include <set>
#include <unordered_set>

#include <range/v3/algorithm/set_algorithm.hpp>
#include <range/v3/iterator/insert_iterators.hpp>
#include <range/v3/range/conversion.hpp>
#include <range/v3/view/filter.hpp>

#include "orm/tiny/tinytypes.hpp"
#include "orm/tiny/types/modelattributes.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Tiny::Utils
{

    /*! Library class for the database attribute. */
    class TINYORM_EXPORT Attribute
    {
        Q_DISABLE_COPY_MOVE(Attribute)

    public:
        /*! Deleted default constructor, this is a pure library class. */
        Attribute() = delete;
        /*! Deleted destructor. */
        ~Attribute() = delete;

        /*! Get all keys from attributes vector. */
        static std::set<QString> keys(const QList<AttributeItem> &attributes);
        /*! Get all keys from attributes map. */
        inline static QList<QVariantMap::key_type> keys(const QVariantMap &attributes);
        /*! Get all keys from relations map. */
        template<AllRelationsConcept ...AllRelations>
        static std::set<typename RelationsContainer<AllRelations...>::key_type>
        keys(const RelationsContainer<AllRelations...> &relations);

        /*! Convert a AttributeItem QList to the QVariantMap. */
        static QVariantMap
        convertVectorToMap(const QList<AttributeItem> &attributes);
        /*! Convert a vector of AttributeItem QLists to the vector of QVariantMaps. */
        static QList<QVariantMap>
        convertVectorsToMaps(const QList<QList<AttributeItem>> &attributesVector);

        /*! Convert a AttributeItem QList to the std::unordered_map. */
        static ModelAttributes
        convertVectorToModelAttributes(const QList<AttributeItem> &attributes);

        /*! Convert a AttributeItem QList to the UpdateItem QList. */
        static QList<UpdateItem>
        convertVectorToUpdateItem(const QList<AttributeItem> &attributes);
        /*! Convert a AttributeItem QList to the UpdateItem QList. */
        static QList<UpdateItem>
        convertVectorToUpdateItem(QList<AttributeItem> &&attributes);

        /*! Remove attributes which have duplicate keys and leave only the last one. */
        static QList<AttributeItem>
        removeDuplicateKeys(const QList<AttributeItem> &attributes);
        /*! Remove attributes which have duplicate keys and leave only the last one. */
        static QList<AttributeItem>
        removeDuplicateKeys(QList<AttributeItem> &&attributes);

        /*! Join attributes and values for firstOrXx methods. */
        static QList<AttributeItem>
        joinAttributesForFirstOr(const QList<WhereItem> &attributes,
                                 const QList<AttributeItem> &values,
                                 const QString &keyName);

        /*! Remove a given attributes from the model attributes vector and return
            a copy. */
        template<typename Model>
        static QList<AttributeItem>
        exceptAttributesForReplicate(const Model &model,
                                     const std::unordered_set<QString> &except = {});

        /*! Compare attributes helper function for the ModelsCollection::sortBy(). */
        template<typename T, typename U>
        requires ranges::totally_ordered_with<T, U>
        static std::strong_ordering compareForSortBy(T &&left, U &&right)
        noexcept(noexcept(std::forward<T>(left) == std::forward<U>(right)) &&
                 noexcept(std::forward<T>(left) < std::forward<U>(right)) &&
                 noexcept(std::forward<T>(left) > std::forward<U>(right)));
        /*! Compare attributes in the descending oder helper function
            for the ModelsCollection::sortBy(). */
        template<typename T, typename U>
        requires ranges::totally_ordered_with<T, U>
        static std::strong_ordering compareForSortByDesc(T &&left, U &&right)
        noexcept(noexcept(std::forward<T>(left) == std::forward<U>(right)) &&
                 noexcept(std::forward<T>(left) < std::forward<U>(right)) &&
                 noexcept(std::forward<T>(left) > std::forward<U>(right)));

        /* Serialization */
        /*! Fix null QVariant bug for QJsonDocument, replace null QVariant-s with
            the QVariant(nullptr). */
        inline static QVariantMap fixQtNullVariantBug(QVariantMap &&attributes);
        /*! Fix null QVariant bug for QJsonDocument, replace null QVariant-s with
            the QVariant(nullptr). */
        inline static QVariantList fixQtNullVariantBug(QVariantList &&attributesList);

    private:
        /* Serialization */
        /*! Fix null QVariant bug for QJsonDocument, replace null QVariant-s with
            the QVariant(nullptr). */
        static void fixQtNullVariantBug(QVariantMap &attributes);
        /*! Fix null QVariant bug for QJsonDocument, replace null QVariant-s with
            the QVariant(nullptr). */
        static void fixQtNullVariantBug(QVariantList &attributesList);
    };

    /* public */

    QList<QVariantMap::key_type> Attribute::keys(const QVariantMap &attributes)
    {
        return attributes.keys();
    }

    template<AllRelationsConcept ...AllRelations>
    std::set<typename RelationsContainer<AllRelations...>::key_type>
    Attribute::keys(const RelationsContainer<AllRelations...> &relations)
    {
        std::set<typename RelationsContainer<AllRelations...>::key_type> keys;

        for (const auto &relation : relations)
            keys.emplace(relation.first);

        return keys;
    }

    template<typename Model>
    QList<AttributeItem>
    Attribute::exceptAttributesForReplicate(const Model &model,
                                            const std::unordered_set<QString> &except)
    {
        std::unordered_set<QString> defaults {
            model.getKeyName(),
            model.getCreatedAtColumn(),
            model.getUpdatedAtColumn(),
        };
        // Remove empty attribute names
        std::erase_if(defaults, [](const auto &attribute)
        {
            return attribute.isEmpty();
        });

        // Merge defaults into except
        std::unordered_set<QString> exceptMerged(defaults.size() + except.size());
        if (!except.empty()) {
            exceptMerged = except;
            exceptMerged.merge(defaults);
        }
        else
            exceptMerged = std::move(defaults);

        // Get all attributes excluding those in the exceptMerged set
        return model.getAttributes()
                | ranges::views::filter([&exceptMerged](const AttributeItem &attribute)
        {
            return !exceptMerged.contains(attribute.key);
        })
                | ranges::to<QList<AttributeItem>>();
    }

    template<typename T, typename U>
    requires ranges::totally_ordered_with<T, U>
    std::strong_ordering Attribute::compareForSortBy(T &&left, U &&right)
    noexcept(noexcept(std::forward<T>(left) == std::forward<U>(right)) &&
             noexcept(std::forward<T>(left) < std::forward<U>(right)) &&
             noexcept(std::forward<T>(left) > std::forward<U>(right)))
    {
        if (std::forward<T>(left) == std::forward<U>(right))
            return std::strong_ordering::equal;
        if (std::forward<T>(left) < std::forward<U>(right))
            return std::strong_ordering::less;
        if (std::forward<T>(left) > std::forward<U>(right))
            return std::strong_ordering::greater;

        Q_UNREACHABLE();
    }

    template<typename T, typename U>
    requires ranges::totally_ordered_with<T, U>
    std::strong_ordering Attribute::compareForSortByDesc(T &&left, U &&right)
    noexcept(noexcept(std::forward<T>(left) == std::forward<U>(right)) &&
             noexcept(std::forward<T>(left) < std::forward<U>(right)) &&
             noexcept(std::forward<T>(left) > std::forward<U>(right)))
    {
        if (std::forward<T>(left) == std::forward<U>(right))
            return std::strong_ordering::equal;
        if (std::forward<T>(left) < std::forward<U>(right))
            return std::strong_ordering::greater;
        if (std::forward<T>(left) > std::forward<U>(right))
            return std::strong_ordering::less;

        Q_UNREACHABLE();
    }

    /* Serialization */

    QVariantMap Attribute::fixQtNullVariantBug(QVariantMap &&attributes)
    {
        fixQtNullVariantBug(attributes);

        return std::move(attributes);
    }

    QVariantList Attribute::fixQtNullVariantBug(QVariantList &&attributesList)
    {
        fixQtNullVariantBug(attributesList);

        return std::move(attributesList);
    }

} // namespace Orm::Tiny::Utils

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_TINY_UTILS_ATTRIBUTE_HPP
