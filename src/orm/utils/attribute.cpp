#include "orm/utils/attribute.hpp"

#include <unordered_set>

#include <range/v3/algorithm/contains.hpp>
#include <range/v3/range/conversion.hpp>
#include <range/v3/view/remove_if.hpp>

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm::Utils::Attribute
{

QVariantMap
convertVectorToMap(const QVector<AttributeItem> &attributes)
{
    QVariantMap result;

    for (const auto &attribute : attributes)
        result.insert(attribute.key, attribute.value);

    return result;
}

QVector<QVariantMap>
convertVectorsToMaps(const QVector<QVector<AttributeItem>> &attributesVector)
{
    const auto size = attributesVector.size();
    QVector<QVariantMap> result(size);

    for (int i = 0; i < size; ++i)
        for (const auto &attribute : attributesVector[i])
            result[i][attribute.key] = attribute.value;

    return result;
}

QVector<UpdateItem>
convertVectorToUpdateItem(const QVector<AttributeItem> &attributes)
{
    QVector<UpdateItem> result;
    result.reserve(attributes.size());

    for (const auto &attribute : attributes)
        result.append({attribute.key, attribute.value});

    return result;
}

QVector<UpdateItem>
convertVectorToUpdateItem(QVector<AttributeItem> &&attributes)
{
    QVector<UpdateItem> result;
    result.reserve(attributes.size());

    for (auto &&attribute : attributes)
        result.append({std::move(attribute.key),
                       std::move(attribute.value)});

    return result;
}

QVector<AttributeItem>
removeDuplicitKeys(const QVector<AttributeItem> &attributes)
{
    const auto size = attributes.size();
    std::unordered_set<QString> added(size);
    QVector<AttributeItem> dedupedAttributes;
    dedupedAttributes.reserve(size);

    // If I want to leave only the last duplicate, I have to loop in the reverse order
    for (auto i = size - 1; i > -1; --i) {
        const auto &attribute = attributes.at(i);
        const auto &key = attribute.key;

        // If duplicit key then skip
        if (added.contains(key))
            continue;

        added.emplace(key);
        dedupedAttributes.append(attribute);
    }

    // Reverse order
    return QVector<AttributeItem>(dedupedAttributes.crbegin(), dedupedAttributes.crend());
}

QVector<AttributeItem>
joinAttributesForFirstOr(const QVector<WhereItem> &attributes,
                         const QVector<AttributeItem> &values)
{
    // Convert attributes to the QVector<AttributeItem>, so they can be joined
    QVector<AttributeItem> attributesConverted(attributes.cbegin(),
                                               attributes.cend());

    // Attributes which already exist in 'attributes' will be removed from 'values'
    using namespace ranges;
    auto valuesFiltered =
            values | views::remove_if(
                [&attributesConverted](const AttributeItem &value)
    {
        return ranges::contains(attributesConverted, true,
                                [&value](const AttributeItem &attribute)
        {
            return attribute.key == value.key;
        });
    })
            | ranges::to<QVector<AttributeItem>>();

    return attributesConverted + valuesFiltered;
}

} // namespace Orm
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif
