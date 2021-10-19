#include "orm/ormtypes.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm
{

WhereItem::operator AttributeItem() const
{
    /* AttributeItem or more precise TinyORM attributes as such, can not contain
       expression in the column name. */
    return {std::get<QString>(column), value};
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

TINYORM_END_COMMON_NAMESPACE
