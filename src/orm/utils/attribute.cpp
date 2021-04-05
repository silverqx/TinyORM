#include "orm/utils/attribute.hpp"

#include <unordered_set>

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

} // namespace Orm
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif
