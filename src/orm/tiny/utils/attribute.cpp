#include "orm/tiny/utils/attribute.hpp"

#include <range/v3/algorithm/contains.hpp>
#include <range/v3/view/reverse.hpp>
#include <range/v3/view/transform.hpp>

#include "orm/macros/likely.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Tiny::Utils
{

/* public */

std::set<QString> Attribute::keys(const QList<AttributeItem> &attributes)
{
    std::set<QString> keys;

    for (const auto &attribute : attributes)
        keys.emplace(attribute.key);

    return keys;
}

QVariantMap Attribute::convertVectorToMap(const QList<AttributeItem> &attributes)
{
    QVariantMap result;

    for (const auto &attribute : attributes)
        result.insert(attribute.key, attribute.value);

    return result;
}

QList<QVariantMap>
Attribute::convertVectorsToMaps(const QList<QList<AttributeItem>> &attributesVector)
{
    const auto size = attributesVector.size();

    QList<QVariantMap> result(size);

    for (QList<QVariantMap>::size_type i = 0; i < size; ++i)
        for (const auto &attribute : attributesVector[i])
            result[i][attribute.key] = attribute.value;

    return result;
}

ModelAttributes
Attribute::convertVectorToModelAttributes(const QList<AttributeItem> &attributes)
{
    ModelAttributes result;
    result.reserve(static_cast<decltype (result)::size_type>(attributes.size()));

    for (const auto &attribute : attributes)
        result.try_emplace(attribute.key, attribute.value);

    return result;
}

QList<UpdateItem>
Attribute::convertVectorToUpdateItem(const QList<AttributeItem> &attributes)
{
    QList<UpdateItem> result;
    result.reserve(attributes.size());

    for (const auto &attribute : attributes)
        result.append({attribute.key, attribute.value});

    return result;
}

QList<UpdateItem>
Attribute::convertVectorToUpdateItem(QList<AttributeItem> &&attributes) // NOLINT(cppcoreguidelines-rvalue-reference-param-not-moved)
{
    QList<UpdateItem> result;
    result.reserve(attributes.size());

    for (auto &&attribute : attributes)
        result.append({std::move(attribute.key),
                       std::move(attribute.value)});

    return result;
}

QList<AttributeItem>
Attribute::removeDuplicateKeys(const QList<AttributeItem> &attributes)
{
    const auto size = attributes.size();

    /*! The helper set, to check duplicate keys. */
    using AddedType = std::unordered_set<QString>;
    AddedType added(static_cast<AddedType::size_type>(size));

    QList<std::reference_wrapper<const AttributeItem>> dedupedAttributesReversed;
    dedupedAttributesReversed.reserve(size);

    /* If I want to get only the last duplicate, I have to loop in the reverse order,
       so the previous attributes will be skipped and only the last attribute
       will be copied. */
    for (const auto &attribute : attributes | ranges::views::reverse) {
        // If duplicate key then skip
        if (added.contains(attribute.key))
            continue;

        added.emplace(attribute.key);
        dedupedAttributesReversed << std::cref(attribute);
    }

    // Materialize the vector of references in reverse order
    return dedupedAttributesReversed
            | ranges::views::reverse
            | ranges::to<QList<AttributeItem>>();
}

QList<AttributeItem>
Attribute::removeDuplicateKeys(QList<AttributeItem> &&attributes) // NOLINT(cppcoreguidelines-rvalue-reference-param-not-moved)
{
    const auto size = attributes.size();

    /*! The helper set, to check duplicate keys. */
    using AddedType = std::unordered_set<QString>;
    AddedType added(static_cast<AddedType::size_type>(size));

    QList<std::reference_wrapper<AttributeItem>> dedupedAttributesReversed;
    dedupedAttributesReversed.reserve(size);

    /* If I want to get only the last duplicate, I have to loop in the reverse order,
       so the previous attributes will be skipped and only the last attribute
       will be moved. */
    for (auto &attribute : attributes | ranges::views::reverse) {
        // If duplicate key then skip
        if (added.contains(attribute.key))
            continue;

        added.emplace(attribute.key);
        dedupedAttributesReversed << std::ref(attribute);
    }

    // Materialize the vector of references in reverse order
    QList<AttributeItem> dedupedAttributes;
    dedupedAttributes.reserve(dedupedAttributesReversed.size());

    for (auto &&attribute : dedupedAttributesReversed | ranges::views::reverse)
        dedupedAttributes << std::move(attribute.get());

    return dedupedAttributes;
}

QList<AttributeItem>
Attribute::joinAttributesForFirstOr(const QList<WhereItem> &attributes,
                                    const QList<AttributeItem> &values,
                                    const QString &keyName)
{
    // Remove the primary key from attributes
    auto attributesFiltered = attributes
            | ranges::views::remove_if([&keyName](const WhereItem &attribute)
    {
        /* AttributeItem or more precise TinyORM attributes as such, can not contain
           expression in the column name.
           Of course, I could obtain the QString value from the expression and
           compare it, but I will not support that intentionally, instead
           the std::bad_variant_access exception will be thrown. */
        return keyName == std::get<QString>(attribute.column);
    })
            | ranges::views::transform([](const WhereItem &attribute)
    {
        /* AttributeItem or more precise TinyORM attributes as such, can not contain
           expression in the column name. */
        return AttributeItem {std::get<QString>(attribute.column), attribute.value};
    })
            | ranges::to<QList<AttributeItem>>();

    // Attributes which already exist in 'attributes' will be removed from 'values'
    auto valuesFiltered = values
            | ranges::views::remove_if([&attributesFiltered](const AttributeItem &value)
    {
        return ranges::contains(attributesFiltered, true,
                                [&value](const AttributeItem &attribute)
        {
            return attribute.key == value.key;
        });
    })
            | ranges::to<QList<AttributeItem>>();

    return attributesFiltered + valuesFiltered;
}

/* private */

/* Serialization */

void Attribute::fixQtNullVariantBug(QVariantMap &attributes) // NOLINT(misc-no-recursion)
{
    for (auto &value : attributes)
        if (value.isNull()) T_UNLIKELY
            value = QVariant::fromValue(nullptr);

        else if (const auto typeId = value.typeId();
                 typeId == QMetaType::QVariantMap
        ) T_UNLIKELY
            fixQtNullVariantBug(
                *reinterpret_cast<QVariantMap *>(value.data())); // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)

        else if (typeId == QMetaType::QVariantList) T_UNLIKELY
            fixQtNullVariantBug(
                *reinterpret_cast<QVariantList *>(value.data())); // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)

        else T_LIKELY
            continue;
}

void Attribute::fixQtNullVariantBug(QVariantList &attributesList) // NOLINT(misc-no-recursion)
{
    for (auto &attributes : attributesList)
        if (attributes.typeId() == QMetaType::QVariantMap) T_LIKELY
            fixQtNullVariantBug(
                *reinterpret_cast<QVariantMap *>(attributes.data())); // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)

        else T_UNLIKELY
            // Don't throw here, just continue normally for Release builds
            Q_UNREACHABLE();
}

} // namespace Orm::Tiny::Utils

TINYORM_END_COMMON_NAMESPACE
