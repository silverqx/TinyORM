#include "orm/tiny/tinytypes.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Tiny
{

/* AttributeItem */

/* public */

AttributeItem::operator UpdateItem() const
{
    return {key, value};
}

bool operator==(const AttributeItem &left, const AttributeItem &right)
{
    return left.key == right.key && left.value == right.value;
}

/* WithItem */

/* public */

bool operator==(const WithItem &left, const WithItem &right)
{
    return left.name == right.name;
}

} // namespace Orm::Tiny

TINYORM_END_COMMON_NAMESPACE
