#ifndef ATTRIBUTE_H
#define ATTRIBUTE_H

#include "orm/ormtypes.h"

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm::Utils::Attribute
{
    /*! Convert a AttributeItem QVector to the QVariantMap. */
    QVariantMap convertVectorToMap(const QVector<AttributeItem> &attributes);
    /*! Convert a AttributeItem QVector to the UpdateItem QVector. */
    QVector<UpdateItem>
    convertVectorToUpdateItem(const QVector<AttributeItem> &attributes);

} // namespace Orm::Utils::Attribute
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif

#endif // ATTRIBUTE_H
