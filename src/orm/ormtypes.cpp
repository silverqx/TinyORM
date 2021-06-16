#include "orm/ormtypes.hpp"

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm
{

WhereItem::operator AttributeItem() const
{
    return {column, value};
}

bool operator==(const AttributeItem &lhs, const AttributeItem &rhs)
{
    return lhs.key == rhs.key && lhs.value == rhs.value;
}

Orm::AttributeItem::operator UpdateItem() const
{
    return {key, value};
}

SyncChanges::SyncChanges()
    : map {{"attached", {}}, {"detached", {}}, {"updated", {}}}
{}

bool SyncChanges::supportedKey(const QString &key) const
{
    return SyncKeys.contains(key);
}

//bool operator==(const WithItem &lhs, const WithItem &rhs)
//{
//    return lhs.name == rhs.name;
//}

} // namespace Orm
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif
