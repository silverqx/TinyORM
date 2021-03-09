#include "orm/utils/attribute.hpp"

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm::Utils::Attribute
{

QVariantMap
convertVectorToMap(const QVector<AttributeItem> &attributes)
{
    // TODO mistake m_attributes/m_original 😭 silverqx
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

} // namespace Orm
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif
