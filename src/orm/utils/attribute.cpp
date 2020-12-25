#include "attribute.h"

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm
{

QVariantMap
Utils::Attribute::convertVectorToMap(const QVector<AttributeItem> &attributes)
{
    // TODO mistake m_attributes/m_original 😭 silverqx
    QVariantMap values;
    for (const auto &attribute : attributes)
        values.insert(attribute.key, attribute.value);

    return values;
}

QVector<UpdateItem>
Utils::Attribute::convertVectorToUpdateItem(const QVector<AttributeItem> &attributes)
{
    QVector<UpdateItem> values;
    for (const auto &attribute : attributes)
        values.append({attribute.key, attribute.value});

    return values;
}

} // namespace Orm
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif
