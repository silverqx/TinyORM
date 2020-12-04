#include "ormtypes.h"

#ifdef MANGO_COMMON_NAMESPACE
namespace MANGO_COMMON_NAMESPACE
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

//bool operator==(const WithItem &lhs, const WithItem &rhs)
//{
//    return (lhs.name == rhs.name);
//}

} // namespace Orm
#ifdef MANGO_COMMON_NAMESPACE
} // namespace MANGO_COMMON_NAMESPACE
#endif
