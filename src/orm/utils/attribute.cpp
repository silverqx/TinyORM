#include "attribute.h"

#ifdef MANGO_COMMON_NAMESPACE
namespace MANGO_COMMON_NAMESPACE
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

} // namespace Orm
#ifdef MANGO_COMMON_NAMESPACE
} // namespace MANGO_COMMON_NAMESPACE
#endif
