#ifndef ATTRIBUTE_H
#define ATTRIBUTE_H

#include "orm/ormtypes.hpp"

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm::Utils::Attribute
{
    /*! Convert a AttributeItem QVector to the QVariantMap. */
    SHAREDLIB_EXPORT QVariantMap
    convertVectorToMap(const QVector<AttributeItem> &attributes);
    /*! Convert a AttributeItem QVector to the UpdateItem QVector. */
    SHAREDLIB_EXPORT QVector<UpdateItem>
    convertVectorToUpdateItem(const QVector<AttributeItem> &attributes);

} // namespace Orm::Utils::Attribute
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif

#endif // ATTRIBUTE_H
