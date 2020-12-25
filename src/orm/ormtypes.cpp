#include "orm/ormtypes.hpp"

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm
{

AssignmentList::AssignmentList(const QVariantHash &variantHash)
{
    auto itHash = variantHash.constBegin();
    while (itHash != variantHash.constEnd()) {
        *this << AssignmentListItem({itHash.key(), itHash.value()});
        ++itHash;
    }
}

WhereItem::operator AttributeItem() const
{
    return {column, value};
}

//bool operator==(const WithItem &lhs, const WithItem &rhs)
//{
//    return (lhs.name == rhs.name);
//}

} // namespace Orm
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif
