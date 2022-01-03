#include "orm/tiny/tinytypes.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Tiny
{

AttributeItem::operator UpdateItem() const
{
    return {key, value};
}

bool operator==(const AttributeItem &lhs, const AttributeItem &rhs)
{
    return lhs.key == rhs.key && lhs.value == rhs.value;
}

bool operator==(const WithItem &lhs, const WithItem &rhs)
{
    return lhs.name == rhs.name;
}

} // namespace Orm::Tiny

TINYORM_END_COMMON_NAMESPACE
